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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_trc.h"
#شامل "bufq.h"
#شامل "select.h"
#شامل "vquic/capsule.h"
#شامل "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

ساختار cf_capsule_ctx {
  ساختار bufq recvbuf;
  ساختار bufq sendbuf;
};

ایستا تهی cf_capsule_destroy(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data)
{
  ساختار cf_capsule_ctx *ctx = cf->ctx;
  (تهی)data;
  اگر(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ایستا CURLcode cf_capsule_connect(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   bool *done)
{
  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }
  اگر(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    اگر(!result && *done)
      cf->connected = TRUE;
    بازگشت result;
  }
  *done = FALSE;
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_capsule_flush(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data)
{
  ساختار cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  اگر(Curl_bufq_is_empty(&ctx->sendbuf))
    بازگشت CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, پوچ, 0,
                             &nwritten);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    بازگشت result;
  }
  بازگشت Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ایستا CURLcode cf_capsule_send(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ثابت uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  ساختار cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (تهی)eos;
  *pnwritten = 0;

  اگر(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    اگر(result)
      بازگشت result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  اگر(result)
    بازگشت result;

  result = cf_capsule_flush(cf, data);
  اگر(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  اگر(!result)
    *pnwritten = len;
  بازگشت result;
}

ایستا CURLcode cf_capsule_recv(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                نویسه *buf, size_t len,
                                size_t *pnread)
{
  ساختار cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  وقتی(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    اگر(result == CURLE_AGAIN)
      بشکن;
    اگر(result)
      بازگشت result;
    اگر(!nread)
      بشکن;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned نویسه *)buf, len,
                                         &result);
  بازگشت result;
}

ایستا bool cf_capsule_data_pending(ساختار Curl_cfilter *cf,
                                    ثابت ساختار Curl_easy *data)
{
  ساختار cf_capsule_ctx *ctx = cf->ctx;

  اگر(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    بازگشت TRUE;
  بازگشت cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ایستا CURLcode cf_capsule_cntrl(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 صحیح event, صحیح arg1, تهی *arg2)
{
  CURLcode result = CURLE_OK;

  (تهی)arg1;
  (تهی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode cf_capsule_query(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_capsule_ctx *ctx = cf->ctx;

  (تهی)pres2;
  انتخاب(query) {
  حالت CF_QUERY_NEED_FLUSH: {
    اگر(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      بازگشت CURLE_OK;
    }
    بشکن;
  }
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ایستا CURLcode cf_capsule_adjust_pollset(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          ساختار easy_pollset *ps)
{
  ساختار cf_capsule_ctx *ctx = cf->ctx;

  اگر(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    اگر(sock != CURL_SOCKET_BAD)
      بازگشت Curl_pollset_add_out(data, ps, sock);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_capsule_shutdown(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  اگر(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  وگرنه {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    اگر(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  بازگشت result;
}

ساختار Curl_cftype Curl_cft_capsule = {
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

ایستا CURLcode cf_capsule_create(ساختار Curl_cfilter **pcf,
                                  ساختار Curl_easy *data,
                                  ساختار connectdata *conn)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_capsule_ctx *ctx;
  CURLcode result;

  (تهی)data;
  (تهی)conn;
  *pcf = پوچ;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : پوچ;
  اگر(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  بازگشت result;
}

CURLcode Curl_cf_capsule_درج_after(ساختار Curl_cfilter *cf_at,
                                      ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  اگر(!result)
    Curl_conn_cf_درج_after(cf_at, cf);
  بازگشت result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
