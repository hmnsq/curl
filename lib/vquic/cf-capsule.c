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
#परिभाषय CAPSULE_RECV_CHUNKS    4
#परिभाषय CAPSULE_SEND_CHUNKS    4
#परिभाषय CAPSULE_CHUNK_आकार     (16 * 1024)

संरचना cf_capsule_ctx {
  संरचना bufq recvbuf;
  संरचना bufq sendbuf;
};

स्थिर शून्य cf_capsule_destroy(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  (शून्य)दत्त;
  यदि(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

स्थिर CURLcode cf_capsule_connect(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   bool *समाप्तम्)
{
  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }
  यदि(cf->अग्र) {
    CURLcode परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(!परिणाम && *समाप्तम्)
      cf->connected = TRUE;
    प्रत्यागम परिणाम;
  }
  *समाप्तम् = FALSE;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_capsule_flush(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nwritten;

  यदि(Curl_bufq_is_रिक्तानि(&ctx->sendbuf))
    प्रत्यागम CURLE_OK;

  परिणाम = Curl_cf_send_bufq(cf->अग्र, दत्त, &ctx->sendbuf, रिक्त, 0,
                             &nwritten);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(दत्त, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    प्रत्यागम परिणाम;
  }
  प्रत्यागम Curl_bufq_is_रिक्तानि(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर CURLcode cf_capsule_send(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                ध्रुव uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  (शून्य)eos;
  *pnwritten = 0;

  यदि(Curl_bufq_is_पूर्णम्(&ctx->sendbuf)) {
    परिणाम = cf_capsule_flush(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* encapsulate new payload into a capsule */
  परिणाम = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = cf_capsule_flush(cf, दत्त);
  यदि(परिणाम == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    परिणाम = CURLE_OK;
  }

  यदि(!परिणाम)
    *pnwritten = len;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_capsule_recv(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                वर्ण *buf, size_t len,
                                size_t *pnread)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  यावत्(!Curl_bufq_is_पूर्णम्(&ctx->recvbuf)) {
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, दत्त, &ctx->recvbuf, 0, &nread);
    यदि(परिणाम == CURLE_AGAIN)
      भङ्ग;
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(!nread)
      भङ्ग;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, दत्त, &ctx->recvbuf,
                                         (अचिह्नित वर्ण *)buf, len,
                                         &परिणाम);
  प्रत्यागम परिणाम;
}

स्थिर bool cf_capsule_दत्त_pending(संरचना Curl_cfilter *cf,
                                    ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  यदि(ctx && !Curl_bufq_is_रिक्तानि(&ctx->recvbuf))
    प्रत्यागम TRUE;
  प्रत्यागम cf->अग्र ? cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त) : FALSE;
}

स्थिर CURLcode cf_capsule_cntrl(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode परिणाम = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_FLUSH:
    परिणाम = cf_capsule_flush(cf, दत्त);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_capsule_query(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  (शून्य)pres2;
  विकल्प(query) {
  पक्ष CF_QUERY_NEED_FLUSH: {
    यदि(!Curl_bufq_is_रिक्तानि(&ctx->sendbuf)) {
      *pres1 = TRUE;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode cf_capsule_adjust_pollset(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त,
                                          संरचना easy_pollset *ps)
{
  संरचना cf_capsule_ctx *ctx = cf->ctx;

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, दत्त);
    यदि(sock != CURL_SOCKET_BAD)
      प्रत्यागम Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, sock);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_capsule_shutdown(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(!cf->connected || cf->shutdown) {
    *समाप्तम् = TRUE;
  }
  अन्यथा {
    परिणाम = cf_capsule_flush(cf, दत्त);
    *समाप्तम् = !परिणाम;
    यदि(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
  }
  प्रत्यागम परिणाम;
}

संरचना Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_दत्त_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

स्थिर CURLcode cf_capsule_create(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_capsule_ctx *ctx;
  CURLcode परिणाम;

  (शून्य)दत्त;
  (शून्य)conn;
  *pcf = रिक्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_आकार, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_सीमा);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_आकार, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_सीमा);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

बहिर्गमनम्:
  *pcf = (!परिणाम) ? cf : रिक्त;
  यदि(परिणाम && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_capsule_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                      संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_capsule_create(&cf, दत्त, cf_at->conn);
  यदि(!परिणाम)
    Curl_conn_cf_निवेशय_after(cf_at, cf);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
