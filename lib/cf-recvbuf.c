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

#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-recvbuf.h"
#अन्तर्भाव "curl_trc.h"

#परिभाषय CURL_CF_RECVBUF_CHUNK     (16 * 1024)

संरचना cf_recvbuf_ctx {
  संरचना bufq recvbuf;
};

स्थिर शून्य cf_recvbuf_destroy(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त)
{
  संरचना cf_recvbuf_ctx *ctx = cf->ctx;
  (शून्य)दत्त;
  यदि(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_recvbuf_recv(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                वर्ण *buf, size_t len,
                                size_t *pnread)
{
  संरचना cf_recvbuf_ctx *ctx = cf->ctx;

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->recvbuf)) {
    प्रत्यागम Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  यदि(cf->अग्र)
    प्रत्यागम cf->अग्र->cft->do_recv(cf->अग्र, दत्त, buf, len, pnread);
  *pnread = 0;
  प्रत्यागम CURLE_RECV_ERROR;
}

स्थिर bool cf_recvbuf_दत्त_pending(संरचना Curl_cfilter *cf,
                                    ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_recvbuf_ctx *ctx = cf->ctx;

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->recvbuf))
    प्रत्यागम TRUE;

  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त) : FALSE;
}

संरचना Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_दत्त_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

स्थिर CURLcode cf_recvbuf_create(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *दत्त,
                                  ध्रुव uint8_t *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_recvbuf_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nwritten = 0;

  (शून्य)दत्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_सीमा | BUFQ_OPT_NO_SPARES));
  परिणाम = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(nwritten != blen) {
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ctx = रिक्त;

बहिर्गमनम्:
  *pcf = परिणाम ? रिक्त : cf;
  यदि(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_recvbuf_सङ्कलय(संरचना Curl_easy *दत्त,
                             संरचना connectdata *conn,
                             int8_t sockindex,
                             ध्रुव uint8_t *buf, size_t blen)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(दत्त);
  परिणाम = cf_recvbuf_create(&cf, दत्त, buf, blen);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_सङ्कलय(दत्त, conn, sockindex, cf);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_WEBSOCKETS */
