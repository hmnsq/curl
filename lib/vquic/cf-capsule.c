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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/capsule.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPSULE_RECV_CHUNKS    4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPSULE_SEND_CHUNKS    4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPSULE_CHUNK_ਮਾਪ     (16 * 1024)

ਬਣਤਰ cf_capsule_ctx {
  ਬਣਤਰ bufq recvbuf;
  ਬਣਤਰ bufq sendbuf;
};

ਸਥਿਰ ਖਾਲੀ cf_capsule_destroy(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_capsule_ctx *ctx = cf->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ਸਥਿਰ CURLcode cf_capsule_connect(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   bool *ਪੂਰਾ)
{
  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
  ਜੇ(cf->ਅਗਲਾ) {
    CURLcode ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ)
      cf->connected = TRUE;
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  *ਪੂਰਾ = FALSE;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_capsule_flush(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nwritten;

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->sendbuf))
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_cf_send_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->sendbuf, ਨਿਰਾ, 0,
                             &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ਸਥਿਰ CURLcode cf_capsule_send(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅਚਲ uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  ਬਣਤਰ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)eos;
  *pnwritten = 0;

  ਜੇ(Curl_bufq_is_ਭਰਿਆ(&ctx->sendbuf)) {
    ਨਤੀਜਾ = cf_capsule_flush(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* encapsulate new payload into a capsule */
  ਨਤੀਜਾ = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_capsule_flush(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    ਨਤੀਜਾ = CURLE_OK;
  }

  ਜੇ(!ਨਤੀਜਾ)
    *pnwritten = len;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_capsule_recv(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅੱਖਰ *buf, size_t len,
                                size_t *pnread)
{
  ਬਣਤਰ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  ਜਦੋਂ(!Curl_bufq_is_ਭਰਿਆ(&ctx->recvbuf)) {
    ਨਤੀਜਾ = Curl_cf_recv_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->recvbuf, 0, &nread);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਰੋਕੋ;
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(!nread)
      ਰੋਕੋ;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, ਅੰਕੜੇ, &ctx->recvbuf,
                                         (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf, len,
                                         &ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_capsule_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                    ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_capsule_ctx *ctx = cf->ctx;

  ਜੇ(ctx && !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->recvbuf))
    ਵਾਪਸ TRUE;
  ਵਾਪਸ cf->ਅਗਲਾ ? cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ) : FALSE;
}

ਸਥਿਰ CURLcode cf_capsule_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_FLUSH:
    ਨਤੀਜਾ = cf_capsule_flush(cf, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_capsule_query(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_capsule_ctx *ctx = cf->ctx;

  (ਖਾਲੀ)pres2;
  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_NEED_FLUSH: {
    ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ CURLcode cf_capsule_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_capsule_ctx *ctx = cf->ctx;

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);
    ਜੇ(sock != CURL_SOCKET_BAD)
      ਵਾਪਸ Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, sock);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_capsule_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!cf->connected || cf->shutdown) {
    *ਪੂਰਾ = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = cf_capsule_flush(cf, ਅੰਕੜੇ);
    *ਪੂਰਾ = !ਨਤੀਜਾ;
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਨਤੀਜਾ = CURLE_OK;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_ਅੰਕੜੇ_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

ਸਥਿਰ CURLcode cf_capsule_create(ਬਣਤਰ Curl_cfilter **pcf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_capsule_ctx *ctx;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;
  *pcf = ਨਿਰਾ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_ਮਾਪ, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_ਮਾਪ, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

ਬਾਹਰਲਾ:
  *pcf = (!ਨਤੀਜਾ) ? cf : ਨਿਰਾ;
  ਜੇ(ਨਤੀਜਾ && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_capsule_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_capsule_create(&cf, ਅੰਕੜੇ, cf_at->conn);
  ਜੇ(!ਨਤੀਜਾ)
    Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
