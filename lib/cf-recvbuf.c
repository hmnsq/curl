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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "bufq.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "cf-recvbuf.h"
#অন্তর্ভাৱ "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

সংরচনা cf_recvbuf_ctx {
  সংরচনা bufq recvbuf;
};

স্থির শূন্য cf_recvbuf_destroy(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data)
{
  সংরচনা cf_recvbuf_ctx *ctx = cf->ctx;
  (শূন্য)data;
  যদি(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

স্থির CURLcode cf_recvbuf_recv(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                ৱর্ণ *buf, size_t len,
                                size_t *pnread)
{
  সংরচনা cf_recvbuf_ctx *ctx = cf->ctx;

  যদি(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    ঘূৰাই Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  যদি(cf->next)
    ঘূৰাই cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ঘূৰাই CURLE_RECV_ERROR;
}

স্থির bool cf_recvbuf_data_pending(সংরচনা Curl_cfilter *cf,
                                    ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা cf_recvbuf_ctx *ctx = cf->ctx;

  যদি(!Curl_bufq_is_empty(&ctx->recvbuf))
    ঘূৰাই TRUE;

  ঘূৰাই cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

সংরচনা Curl_cftype Curl_cft_recvbuf = {
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

স্থির CURLcode cf_recvbuf_create(সংরচনা Curl_cfilter **pcf,
                                  সংরচনা Curl_easy *data,
                                  ধ্রুৱ uint8_t *buf, size_t blen)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (শূন্য)data;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  যদি(result)
    গচ্ছ out;
  যদি(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    গচ্ছ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  যদি(result)
    গচ্ছ out;
  ctx = রিক্ত;

out:
  *pcf = result ? রিক্ত : cf;
  যদি(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  ঘূৰাই result;
}

CURLcode Curl_cf_recvbuf_add(সংরচনা Curl_easy *data,
                             সংরচনা connectdata *conn,
                             int8_t sockindex,
                             ধ্রুৱ uint8_t *buf, size_t blen)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  যদি(result)
    গচ্ছ out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ঘূৰাই result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
