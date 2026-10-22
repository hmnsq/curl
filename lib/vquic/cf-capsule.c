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

#अगर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "curl_trc.h"
#समावेश "bufq.h"
#समावेश "select.h"
#समावेश "vquic/capsule.h"
#समावेश "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#घोषणा CAPSULE_RECV_CHUNKS    4
#घोषणा CAPSULE_SEND_CHUNKS    4
#घोषणा CAPSULE_CHUNK_आकार     (16 * 1024)

काष्ठा cf_capsule_ctx {
  काष्ठा bufq recvbuf;
  काष्ठा bufq sendbuf;
};

अटल व्योम cf_capsule_destroy(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_capsule_ctx *ctx = cf->ctx;
  (व्योम)तथ्य;
  अगर(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

अटल CURLcode cf_capsule_connect(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   bool *done)
{
  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }
  अगर(cf->अग्र) {
    CURLcode परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, तथ्य, done);
    अगर(!परिणाम && *done)
      cf->connected = TRUE;
    वापस परिणाम;
  }
  *done = FALSE;
  वापस CURLE_OK;
}

अटल CURLcode cf_capsule_flush(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_capsule_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nwritten;

  अगर(Curl_bufq_is_empty(&ctx->sendbuf))
    वापस CURLE_OK;

  परिणाम = Curl_cf_send_bufq(cf->अग्र, तथ्य, &ctx->sendbuf, शून्य, 0,
                             &nwritten);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(तथ्य, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    वापस परिणाम;
  }
  वापस Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

अटल CURLcode cf_capsule_send(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                स्थिर uint8_t *buf, माप_प्रकार len,
                                bool eos, माप_प्रकार *pnwritten)
{
  काष्ठा cf_capsule_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  (व्योम)eos;
  *pnwritten = 0;

  अगर(Curl_bufq_is_full(&ctx->sendbuf)) {
    परिणाम = cf_capsule_flush(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* encapsulate new payload into a capsule */
  परिणाम = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = cf_capsule_flush(cf, तथ्य);
  अगर(परिणाम == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    परिणाम = CURLE_OK;
  }

  अगर(!परिणाम)
    *pnwritten = len;
  वापस परिणाम;
}

अटल CURLcode cf_capsule_recv(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                अक्षर *buf, माप_प्रकार len,
                                माप_प्रकार *pnread)
{
  काष्ठा cf_capsule_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  माप_प्रकार nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  जबतक(!Curl_bufq_is_full(&ctx->recvbuf)) {
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, तथ्य, &ctx->recvbuf, 0, &nread);
    अगर(परिणाम == CURLE_AGAIN)
      अवरोध;
    अगर(परिणाम)
      वापस परिणाम;
    अगर(!nread)
      अवरोध;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, तथ्य, &ctx->recvbuf,
                                         (अचिन्हित अक्षर *)buf, len,
                                         &परिणाम);
  वापस परिणाम;
}

अटल bool cf_capsule_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                    स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_capsule_ctx *ctx = cf->ctx;

  अगर(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    वापस TRUE;
  वापस cf->अग्र ? cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य) : FALSE;
}

अटल CURLcode cf_capsule_cntrl(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  CURLcode परिणाम = CURLE_OK;

  (व्योम)arg1;
  (व्योम)arg2;
  चयन(event) {
  हाल CF_CTRL_FLUSH:
    परिणाम = cf_capsule_flush(cf, तथ्य);
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode cf_capsule_query(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_capsule_ctx *ctx = cf->ctx;

  (व्योम)pres2;
  चयन(query) {
  हाल CF_QUERY_NEED_FLUSH: {
    अगर(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      वापस CURLE_OK;
    }
    अवरोध;
  }
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

अटल CURLcode cf_capsule_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          काष्ठा easy_pollset *ps)
{
  काष्ठा cf_capsule_ctx *ctx = cf->ctx;

  अगर(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, तथ्य);
    अगर(sock != CURL_SOCKET_BAD)
      वापस Curl_pollset_add_out(तथ्य, ps, sock);
  }
  वापस CURLE_OK;
}

अटल CURLcode cf_capsule_shutdown(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  अन्यथा {
    परिणाम = cf_capsule_flush(cf, तथ्य);
    *done = !परिणाम;
    अगर(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
  }
  वापस परिणाम;
}

काष्ठा Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_तथ्य_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

अटल CURLcode cf_capsule_create(काष्ठा Curl_cfilter **pcf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_capsule_ctx *ctx;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  (व्योम)conn;
  *pcf = शून्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_आकार, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_आकार, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!परिणाम) ? cf : शून्य;
  अगर(परिणाम && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  वापस परिणाम;
}

CURLcode Curl_cf_capsule_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_capsule_create(&cf, तथ्य, cf_at->conn);
  अगर(!परिणाम)
    Curl_conn_cf_जोड़ो_after(cf_at, cf);
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
