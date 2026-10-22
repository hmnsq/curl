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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "vquic/capsule.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

ಸಂರಚನಾ cf_capsule_ctx {
  ಸಂರಚನಾ bufq recvbuf;
  ಸಂರಚನಾ bufq sendbuf;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_capsule_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_capsule_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)data;
  ಯದಿ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_capsule_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   bool *done)
{
  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಯದಿ(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    ಯದಿ(!result && *done)
      cf->connected = TRUE;
    ಪ್ರತ್ಯಾಗಮ result;
  }
  *done = FALSE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_capsule_flush(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ಯದಿ(Curl_bufq_is_empty(&ctx->sendbuf))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, ರಿಕ್ತ, 0,
                             &nwritten);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ಸ್ಥಿರ CURLcode cf_capsule_send(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಧ್ರುವ uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  ಸಂರಚನಾ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಯದಿ(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = cf_capsule_flush(cf, data);
  ಯದಿ(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  ಯದಿ(!result)
    *pnwritten = len;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_capsule_recv(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ವರ್ಣ *buf, size_t len,
                                size_t *pnread)
{
  ಸಂರಚನಾ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  ಯಾವತ್(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    ಯದಿ(result == CURLE_AGAIN)
      ಭಙ್ಗ;
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(!nread)
      ಭಙ್ಗ;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned ವರ್ಣ *)buf, len,
                                         &result);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_capsule_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_capsule_ctx *ctx = cf->ctx;

  ಯದಿ(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ಸ್ಥಿರ CURLcode cf_capsule_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_capsule_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_capsule_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)pres2;
  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_NEED_FLUSH: {
    ಯದಿ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode cf_capsule_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_capsule_ctx *ctx = cf->ctx;

  ಯದಿ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    ಯದಿ(sock != CURL_SOCKET_BAD)
      ಪ್ರತ್ಯಾಗಮ Curl_pollset_add_out(data, ps, sock);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_capsule_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  ಅನ್ಯಥಾ {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    ಯದಿ(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_capsule = {
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

ಸ್ಥಿರ CURLcode cf_capsule_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_capsule_ctx *ctx;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)conn;
  *pcf = ರಿಕ್ತ;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : ರಿಕ್ತ;
  ಯದಿ(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_capsule_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  ಯದಿ(!result)
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
