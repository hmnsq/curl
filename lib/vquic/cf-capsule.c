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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "vquic/capsule.h"
#அந்தர்பாவ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

கட்டமைப்பு cf_capsule_ctx {
  கட்டமைப்பு bufq recvbuf;
  கட்டமைப்பு bufq sendbuf;
};

நிலையான வெறுமை cf_capsule_destroy(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_capsule_ctx *ctx = cf->ctx;
  (வெறுமை)data;
  எனில்(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

நிலையான CURLcode cf_capsule_connect(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   bool *done)
{
  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }
  எனில்(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    எனில்(!result && *done)
      cf->connected = TRUE;
    திரும்பு result;
  }
  *done = FALSE;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_capsule_flush(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  எனில்(Curl_bufq_is_empty(&ctx->sendbuf))
    திரும்பு CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, வெற்று, 0,
                             &nwritten);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    திரும்பு result;
  }
  திரும்பு Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

நிலையான CURLcode cf_capsule_send(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                மாறிலி uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  கட்டமைப்பு cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (வெறுமை)eos;
  *pnwritten = 0;

  எனில்(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  எனில்(result)
    திரும்பு result;

  result = cf_capsule_flush(cf, data);
  எனில்(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  எனில்(!result)
    *pnwritten = len;
  திரும்பு result;
}

நிலையான CURLcode cf_capsule_recv(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                எழுத்து *buf, size_t len,
                                size_t *pnread)
{
  கட்டமைப்பு cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  போது(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    எனில்(result == CURLE_AGAIN)
      நிறுத்து;
    எனில்(result)
      திரும்பு result;
    எனில்(!nread)
      நிறுத்து;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned எழுத்து *)buf, len,
                                         &result);
  திரும்பு result;
}

நிலையான bool cf_capsule_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                    மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_capsule_ctx *ctx = cf->ctx;

  எனில்(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    திரும்பு TRUE;
  திரும்பு cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

நிலையான CURLcode cf_capsule_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 முழு event, முழு arg1, வெறுமை *arg2)
{
  CURLcode result = CURLE_OK;

  (வெறுமை)arg1;
  (வெறுமை)arg2;
  தேர்வு(event) {
  நிலை CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_capsule_query(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_capsule_ctx *ctx = cf->ctx;

  (வெறுமை)pres2;
  தேர்வு(query) {
  நிலை CF_QUERY_NEED_FLUSH: {
    எனில்(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

நிலையான CURLcode cf_capsule_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_capsule_ctx *ctx = cf->ctx;

  எனில்(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    எனில்(sock != CURL_SOCKET_BAD)
      திரும்பு Curl_pollset_add_out(data, ps, sock);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_capsule_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  எனில்(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  இல்லையெனில் {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    எனில்(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  திரும்பு result;
}

கட்டமைப்பு Curl_cftype Curl_cft_capsule = {
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

நிலையான CURLcode cf_capsule_create(கட்டமைப்பு Curl_cfilter **pcf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_capsule_ctx *ctx;
  CURLcode result;

  (வெறுமை)data;
  (வெறுமை)conn;
  *pcf = வெற்று;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : வெற்று;
  எனில்(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  திரும்பு result;
}

CURLcode Curl_cf_capsule_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  எனில்(!result)
    Curl_conn_cf_செருகு_after(cf_at, cf);
  திரும்பு result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
