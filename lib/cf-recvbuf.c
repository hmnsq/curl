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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-recvbuf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CF_RECVBUF_CHUNK     (16 * 1024)

ರಚನೆ cf_recvbuf_ctx {
  ರಚನೆ bufq recvbuf;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_recvbuf_destroy(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_recvbuf_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_recvbuf_recv(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಅಕ್ಷರ *buf, size_t len,
                                size_t *pnread)
{
  ರಚನೆ cf_recvbuf_ctx *ctx = cf->ctx;

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->recvbuf)) {
    ಹಿಂತಿರುಗಿ Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  ಆದರೆ(cf->ಮುಂದಿನ)
    ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ->cft->do_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, len, pnread);
  *pnread = 0;
  ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
}

ಸ್ಥಿರ bool cf_recvbuf_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_recvbuf_ctx *ctx = cf->ctx;

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->recvbuf))
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) : FALSE;
}

ರಚನೆ Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ಸ್ಥಿರ CURLcode cf_recvbuf_create(ರಚನೆ Curl_cfilter **pcf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t blen)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_recvbuf_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nwritten = 0;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_ಎಲ್ಲೆ | BUFQ_OPT_NO_SPARES));
  ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(nwritten != blen) {
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = ಖಾಲಿ;

ಹೊರಹೋಗು:
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  ಆದರೆ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_recvbuf_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn,
                             int8_t sockindex,
                             ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t blen)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = cf_recvbuf_create(&cf, ದತ್ತಾಂಶ, buf, blen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, cf);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_WEBSOCKETS */
