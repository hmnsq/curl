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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-recvbuf.h"
#अन्तर्भाव "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

संरचना cf_recvbuf_ctx {
  संरचना bufq recvbuf;
};

स्थिर शून्य cf_recvbuf_destroy(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data)
{
  संरचना cf_recvbuf_ctx *ctx = cf->ctx;
  (शून्य)data;
  जर(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_recvbuf_recv(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                अक्षर *buf, माप_प्रकार len,
                                माप_प्रकार *pnread)
{
  संरचना cf_recvbuf_ctx *ctx = cf->ctx;

  जर(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    परत Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  जर(cf->next)
    परत cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  परत CURLE_RECV_ERROR;
}

स्थिर bool cf_recvbuf_data_pending(संरचना Curl_cfilter *cf,
                                    स्थिरांक संरचना Curl_easy *data)
{
  संरचना cf_recvbuf_ctx *ctx = cf->ctx;

  जर(!Curl_bufq_is_empty(&ctx->recvbuf))
    परत TRUE;

  परत cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

संरचना Curl_cftype Curl_cft_recvbuf = {
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

स्थिर CURLcode cf_recvbuf_create(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *data,
                                  स्थिरांक uint8_t *buf, माप_प्रकार blen)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार nwritten = 0;

  (शून्य)data;
  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  जर(result)
    जा out;
  जर(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    जा out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  जर(result)
    जा out;
  ctx = रिक्त;

out:
  *pcf = result ? रिक्त : cf;
  जर(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  परत result;
}

CURLcode Curl_cf_recvbuf_add(संरचना Curl_easy *data,
                             संरचना connectdata *conn,
                             int8_t sockindex,
                             स्थिरांक uint8_t *buf, माप_प्रकार blen)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  जर(result)
    जा out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  परत result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
