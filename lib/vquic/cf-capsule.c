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
#ضمن "curl_setup.h"

#𐤀𐤌 !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_trc.h"
#ضمن "bufq.h"
#ضمن "select.h"
#ضمن "vquic/capsule.h"
#ضمن "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

بنية cf_capsule_ctx {
  بنية bufq recvbuf;
  بنية bufq sendbuf;
};

ساكن فراغ cf_capsule_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  (فراغ)data;
  𐤀𐤌(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ساكن CURLcode cf_capsule_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   bool *done)
{
  𐤀𐤌(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }
  𐤀𐤌(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    𐤀𐤌(!result && *done)
      cf->connected = TRUE;
    أرجع result;
  }
  *done = FALSE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_flush(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  𐤀𐤌(Curl_bufq_is_empty(&ctx->sendbuf))
    أرجع CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, عدم, 0,
                             &nwritten);
  𐤀𐤌(result) {
    𐤀𐤌(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    أرجع result;
  }
  أرجع Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ساكن CURLcode cf_capsule_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                ثابت uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (فراغ)eos;
  *pnwritten = 0;

  𐤀𐤌(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    𐤀𐤌(result)
      أرجع result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  𐤀𐤌(result)
    أرجع result;

  result = cf_capsule_flush(cf, data);
  𐤀𐤌(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  𐤀𐤌(!result)
    *pnwritten = len;
  أرجع result;
}

ساكن CURLcode cf_capsule_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  طالما(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    𐤀𐤌(result == CURLE_AGAIN)
      اكسر;
    𐤀𐤌(result)
      أرجع result;
    𐤀𐤌(!nread)
      اكسر;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned حرف *)buf, len,
                                         &result);
  أرجع result;
}

ساكن bool cf_capsule_data_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *data)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  𐤀𐤌(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    أرجع TRUE;
  أرجع cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ساكن CURLcode cf_capsule_cntrl(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode result = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع result;
}

ساكن CURLcode cf_capsule_query(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  (فراغ)pres2;
  بدل(query) {
  حالة CF_QUERY_NEED_FLUSH: {
    𐤀𐤌(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_capsule_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data,
                                          بنية easy_pollset *ps)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  𐤀𐤌(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    𐤀𐤌(sock != CURL_SOCKET_BAD)
      أرجع Curl_pollset_add_out(data, ps, sock);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_shutdown(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  𐤀𐤌(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  وإلا {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    𐤀𐤌(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  أرجع result;
}

بنية Curl_cftype Curl_cft_capsule = {
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

ساكن CURLcode cf_capsule_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *data,
                                  بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_capsule_ctx *ctx;
  CURLcode result;

  (فراغ)data;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  𐤀𐤌(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : عدم;
  𐤀𐤌(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  أرجع result;
}

CURLcode Curl_cf_capsule_insert_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *data)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  𐤀𐤌(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  أرجع result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
