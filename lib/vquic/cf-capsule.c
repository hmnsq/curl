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

#यदि !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

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
  यदि(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

स्थिर CURLcode cf_capsule_connect(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   bool *done)
{
  यदि(cf->connected) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }
  यदि(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    यदि(!result && *done)
      cf->connected = TRUE;
    प्रत्यागम result;
  }
  *done = FALSE;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_capsule_flush(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  यदि(Curl_bufq_is_empty(&ctx->sendbuf))
    प्रत्यागम CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, रिक्त, 0,
                             &nwritten);
  यदि(result) {
    यदि(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    प्रत्यागम result;
  }
  प्रत्यागम Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर CURLcode cf_capsule_send(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                ध्रुव uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (शून्य)eos;
  *pnwritten = 0;

  यदि(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    यदि(result)
      प्रत्यागम result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  यदि(result)
    प्रत्यागम result;

  result = cf_capsule_flush(cf, data);
  यदि(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  यदि(!result)
    *pnwritten = len;
  प्रत्यागम result;
}

स्थिर CURLcode cf_capsule_recv(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                वर्ण *buf, size_t len,
                                size_t *pnread)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  यावत्(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    यदि(result == CURLE_AGAIN)
      भङ्ग;
    यदि(result)
      प्रत्यागम result;
    यदि(!nread)
      भङ्ग;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned वर्ण *)buf, len,
                                         &result);
  प्रत्यागम result;
}

स्थिर bool cf_capsule_data_pending(संरचना Curl_cfilter *cf,
                                    ध्रुव संरचना Curl_easy *data)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  यदि(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    प्रत्यागम TRUE;
  प्रत्यागम cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

स्थिर CURLcode cf_capsule_cntrl(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode cf_capsule_query(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  (शून्य)pres2;
  विकल्प(query) {
  पक्ष CF_QUERY_NEED_FLUSH: {
    यदि(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode cf_capsule_adjust_pollset(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          संरचना easy_pollset *ps)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  यदि(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    यदि(sock != CURL_SOCKET_BAD)
      प्रत्यागम Curl_pollset_add_out(data, ps, sock);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_capsule_shutdown(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  यदि(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  अन्यथा {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    यदि(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  प्रत्यागम result;
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
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : रिक्त;
  यदि(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  प्रत्यागम result;
}

CURLcode Curl_cf_capsule_जोड़ो_after(संरचना Curl_cfilter *cf_at,
                                      संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  यदि(!result)
    Curl_conn_cf_जोड़ो_after(cf_at, cf);
  प्रत्यागम result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
