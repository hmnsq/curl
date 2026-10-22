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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#समावेश "urldata.h"
#समावेश "bufq.h"
#समावेश "cfilters.h"
#समावेश "cf-recvbuf.h"
#समावेश "curl_trc.h"

#घोषणा CURL_CF_RECVBUF_CHUNK     (16 * 1024)

काष्ठा cf_recvbuf_ctx {
  काष्ठा bufq recvbuf;
};

अटल व्योम cf_recvbuf_destroy(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_recvbuf_ctx *ctx = cf->ctx;
  (व्योम)तथ्य;
  अगर(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

अटल CURLcode cf_recvbuf_recv(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                अक्षर *buf, माप_प्रकार len,
                                माप_प्रकार *pnread)
{
  काष्ठा cf_recvbuf_ctx *ctx = cf->ctx;

  अगर(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    वापस Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  अगर(cf->अग्र)
    वापस cf->अग्र->cft->do_recv(cf->अग्र, तथ्य, buf, len, pnread);
  *pnread = 0;
  वापस CURLE_RECV_ERROR;
}

अटल bool cf_recvbuf_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                    स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_recvbuf_ctx *ctx = cf->ctx;

  अगर(!Curl_bufq_is_empty(&ctx->recvbuf))
    वापस TRUE;

  वापस cf->अग्र ?
    cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य) : FALSE;
}

काष्ठा Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_तथ्य_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

अटल CURLcode cf_recvbuf_create(काष्ठा Curl_cfilter **pcf,
                                  काष्ठा Curl_easy *तथ्य,
                                  स्थिर uint8_t *buf, माप_प्रकार blen)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_recvbuf_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nwritten = 0;

  (व्योम)तथ्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  परिणाम = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  अगर(परिणाम)
    जाओ out;
  अगर(nwritten != blen) {
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }

  परिणाम = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  अगर(परिणाम)
    जाओ out;
  ctx = शून्य;

out:
  *pcf = परिणाम ? शून्य : cf;
  अगर(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  वापस परिणाम;
}

CURLcode Curl_cf_recvbuf_add(काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn,
                             int8_t sockindex,
                             स्थिर uint8_t *buf, माप_प्रकार blen)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(तथ्य);
  परिणाम = cf_recvbuf_create(&cf, तथ्य, buf, blen);
  अगर(परिणाम)
    जाओ out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(तथ्य, conn, sockindex, cf);
out:
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_WEBSOCKETS */
