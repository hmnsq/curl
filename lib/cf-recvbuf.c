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

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-recvbuf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

ಸಂರಚನಾ cf_recvbuf_ctx {
  ಸಂರಚನಾ bufq recvbuf;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_recvbuf_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_recvbuf_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)data;
  ಯದಿ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_recvbuf_recv(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ವರ್ಣ *buf, size_t len,
                                size_t *pnread)
{
  ಸಂರಚನಾ cf_recvbuf_ctx *ctx = cf->ctx;

  ಯದಿ(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    ಪ್ರತ್ಯಾಗಮ Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  ಯದಿ(cf->next)
    ಪ್ರತ್ಯಾಗಮ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
}

ಸ್ಥಿರ bool cf_recvbuf_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_recvbuf_ctx *ctx = cf->ctx;

  ಯದಿ(!Curl_bufq_is_empty(&ctx->recvbuf))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_data_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ಸ್ಥಿರ CURLcode cf_recvbuf_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ uint8_t *buf, size_t blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (ಶೂನ್ಯ)data;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ಯದಿ(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ctx = ರಿಕ್ತ;

out:
  *pcf = result ? ರಿಕ್ತ : cf;
  ಯದಿ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_recvbuf_add(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn,
                             int8_t sockindex,
                             ಧ್ರುವ uint8_t *buf, size_t blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
