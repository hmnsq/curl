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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "vquic/capsule.h"
#અન્તર્ભાવ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

સંરચના cf_capsule_ctx {
  સંરચના bufq recvbuf;
  સંરચના bufq sendbuf;
};

સ્થિર શૂન્ય cf_capsule_destroy(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data)
{
  સંરચના cf_capsule_ctx *ctx = cf->ctx;
  (શૂન્ય)data;
  જો(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

સ્થિર CURLcode cf_capsule_connect(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   bool *done)
{
  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }
  જો(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    જો(!result && *done)
      cf->connected = TRUE;
    પાછા result;
  }
  *done = FALSE;
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_capsule_flush(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data)
{
  સંરચના cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  જો(Curl_bufq_is_empty(&ctx->sendbuf))
    પાછા CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, ખાલી, 0,
                             &nwritten);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    પાછા result;
  }
  પાછા Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

સ્થિર CURLcode cf_capsule_send(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                અચલ uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  સંરચના cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (શૂન્ય)eos;
  *pnwritten = 0;

  જો(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    જો(result)
      પાછા result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  જો(result)
    પાછા result;

  result = cf_capsule_flush(cf, data);
  જો(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  જો(!result)
    *pnwritten = len;
  પાછા result;
}

સ્થિર CURLcode cf_capsule_recv(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                અક્ષર *buf, size_t len,
                                size_t *pnread)
{
  સંરચના cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  જ્યારે(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    જો(result == CURLE_AGAIN)
      તોડો;
    જો(result)
      પાછા result;
    જો(!nread)
      તોડો;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned અક્ષર *)buf, len,
                                         &result);
  પાછા result;
}

સ્થિર bool cf_capsule_data_pending(સંરચના Curl_cfilter *cf,
                                    અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_capsule_ctx *ctx = cf->ctx;

  જો(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    પાછા TRUE;
  પાછા cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

સ્થિર CURLcode cf_capsule_cntrl(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  CURLcode result = CURLE_OK;

  (શૂન્ય)arg1;
  (શૂન્ય)arg2;
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode cf_capsule_query(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_capsule_ctx *ctx = cf->ctx;

  (શૂન્ય)pres2;
  પસંદગી(query) {
  સ્થિતિ CF_QUERY_NEED_FLUSH: {
    જો(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      પાછા CURLE_OK;
    }
    તોડો;
  }
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સ્થિર CURLcode cf_capsule_adjust_pollset(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          સંરચના easy_pollset *ps)
{
  સંરચના cf_capsule_ctx *ctx = cf->ctx;

  જો(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    જો(sock != CURL_SOCKET_BAD)
      પાછા Curl_pollset_add_out(data, ps, sock);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_capsule_shutdown(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  જો(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  નહીંતર {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    જો(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  પાછા result;
}

સંરચના Curl_cftype Curl_cft_capsule = {
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

સ્થિર CURLcode cf_capsule_create(સંરચના Curl_cfilter **pcf,
                                  સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_capsule_ctx *ctx;
  CURLcode result;

  (શૂન્ય)data;
  (શૂન્ય)conn;
  *pcf = ખાલી;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : ખાલી;
  જો(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  પાછા result;
}

CURLcode Curl_cf_capsule_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                      સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  જો(!result)
    Curl_conn_cf_ઉમેરો_after(cf_at, cf);
  પાછા result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
