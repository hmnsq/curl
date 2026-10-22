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

#जर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "vquic/capsule.h"
#अन्तर्भाव "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

संरचना cf_capsule_ctx {
  संरचना bufq recvbuf;
  संरचना bufq sendbuf;
};

स्थिर शून्य cf_capsule_destroy(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  (शून्य)data;
  जर(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

स्थिर CURLcode cf_capsule_connect(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   bool *done)
{
  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }
  जर(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    जर(!result && *done)
      cf->connected = TRUE;
    परत result;
  }
  *done = FALSE;
  परत CURLE_OK;
}

स्थिर CURLcode cf_capsule_flush(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार nwritten;

  जर(Curl_bufq_is_empty(&ctx->sendbuf))
    परत CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, रिक्त, 0,
                             &nwritten);
  जर(result) {
    जर(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    परत result;
  }
  परत Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर CURLcode cf_capsule_send(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                स्थिरांक uint8_t *buf, माप_प्रकार len,
                                bool eos, माप_प्रकार *pnwritten)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (शून्य)eos;
  *pnwritten = 0;

  जर(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    जर(result)
      परत result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  जर(result)
    परत result;

  result = cf_capsule_flush(cf, data);
  जर(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  जर(!result)
    *pnwritten = len;
  परत result;
}

स्थिर CURLcode cf_capsule_recv(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                अक्षर *buf, माप_प्रकार len,
                                माप_प्रकार *pnread)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  माप_प्रकार nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  पर्यंत(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    जर(result == CURLE_AGAIN)
      थांबा;
    जर(result)
      परत result;
    जर(!nread)
      थांबा;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (अचिन्हित अक्षर *)buf, len,
                                         &result);
  परत result;
}

स्थिर bool cf_capsule_data_pending(संरचना Curl_cfilter *cf,
                                    स्थिरांक संरचना Curl_easy *data)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  जर(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    परत TRUE;
  परत cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

स्थिर CURLcode cf_capsule_cntrl(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 पूर्णांक event, पूर्णांक arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  निवड(event) {
  स्थिती CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत result;
}

स्थिर CURLcode cf_capsule_query(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  (शून्य)pres2;
  निवड(query) {
  स्थिती CF_QUERY_NEED_FLUSH: {
    जर(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      परत CURLE_OK;
    }
    थांबा;
  }
  पूर्वनिर्धारित:
    थांबा;
  }
  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode cf_capsule_adjust_pollset(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          संरचना easy_pollset *ps)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  जर(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    जर(sock != CURL_SOCKET_BAD)
      परत Curl_pollset_add_out(data, ps, sock);
  }
  परत CURLE_OK;
}

स्थिर CURLcode cf_capsule_shutdown(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  जर(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  अन्यथा {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    जर(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  परत result;
}

संरचना Curl_cftype Curl_cft_capsule = {
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

स्थिर CURLcode cf_capsule_create(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *data,
                                  संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_capsule_ctx *ctx;
  CURLcode result;

  (शून्य)data;
  (शून्य)conn;
  *pcf = रिक्त;
  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : रिक्त;
  जर(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  परत result;
}

CURLcode Curl_cf_capsule_घाला_after(संरचना Curl_cfilter *cf_at,
                                      संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  जर(!result)
    Curl_conn_cf_घाला_after(cf_at, cf);
  परत result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
