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

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "vquic/capsule.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#ವ್ಯಾಖ್ಯಾನಿಸು CAPSULE_RECV_CHUNKS    4
#ವ್ಯಾಖ್ಯಾನಿಸು CAPSULE_SEND_CHUNKS    4
#ವ್ಯಾಖ್ಯಾನಿಸು CAPSULE_CHUNK_ಗಾತ್ರ     (16 * 1024)

ರಚನೆ cf_capsule_ctx {
  ರಚನೆ bufq recvbuf;
  ರಚನೆ bufq sendbuf;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_capsule_destroy(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_capsule_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_capsule_connect(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   bool *ಮುಗಿದ)
{
  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಆದರೆ(cf->ಮುಂದಿನ) {
    CURLcode ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ)
      cf->connected = TRUE;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  *ಮುಗಿದ = FALSE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_capsule_flush(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nwritten;

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->sendbuf))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_cf_send_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->sendbuf, ಖಾಲಿ, 0,
                             &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ಸ್ಥಿರ CURLcode cf_capsule_send(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  ರಚನೆ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಆದರೆ(Curl_bufq_is_ತುಂಬಿದ(&ctx->sendbuf)) {
    ಫಲಿತಾಂಶ = cf_capsule_flush(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* encapsulate new payload into a capsule */
  ಫಲಿತಾಂಶ = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_capsule_flush(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    ಫಲಿತಾಂಶ = CURLE_OK;
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    *pnwritten = len;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_capsule_recv(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಅಕ್ಷರ *buf, size_t len,
                                size_t *pnread)
{
  ರಚನೆ cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  ಆಗ(!Curl_bufq_is_ತುಂಬಿದ(&ctx->recvbuf)) {
    ಫಲಿತಾಂಶ = Curl_cf_recv_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->recvbuf, 0, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(!nread)
      ನಿಲ್ಲಿಸು;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, ದತ್ತಾಂಶ, &ctx->recvbuf,
                                         (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf, len,
                                         &ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_capsule_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_capsule_ctx *ctx = cf->ctx;

  ಆದರೆ(ctx && !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->recvbuf))
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ? cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) : FALSE;
}

ಸ್ಥಿರ CURLcode cf_capsule_cntrl(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_FLUSH:
    ಫಲಿತಾಂಶ = cf_capsule_flush(cf, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_capsule_query(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_capsule_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)pres2;
  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_NEED_FLUSH: {
    ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode cf_capsule_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_capsule_ctx *ctx = cf->ctx;

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(sock != CURL_SOCKET_BAD)
      ಹಿಂತಿರುಗಿ Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, sock);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_capsule_shutdown(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!cf->connected || cf->shutdown) {
    *ಮುಗಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = cf_capsule_flush(cf, ದತ್ತಾಂಶ);
    *ಮುಗಿದ = !ಫಲಿತಾಂಶ;
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಫಲಿತಾಂಶ = CURLE_OK;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_ದತ್ತಾಂಶ_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

ಸ್ಥಿರ CURLcode cf_capsule_create(ರಚನೆ Curl_cfilter **pcf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_capsule_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;
  *pcf = ಖಾಲಿ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_ಗಾತ್ರ, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_ಗಾತ್ರ, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_ಎಲ್ಲೆ);

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

ಹೊರಹೋಗು:
  *pcf = (!ಫಲಿತಾಂಶ) ? cf : ಖಾಲಿ;
  ಆದರೆ(ಫಲಿತಾಂಶ && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_capsule_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_capsule_create(&cf, ದತ್ತಾಂಶ, cf_at->conn);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
