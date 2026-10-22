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

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "bufq.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "vquic/capsule.h"
#অন্তর্ভাৱ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

সংরচনা cf_capsule_ctx {
  সংরচনা bufq recvbuf;
  সংরচনা bufq sendbuf;
};

স্থির শূন্য cf_capsule_destroy(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data)
{
  সংরচনা cf_capsule_ctx *ctx = cf->ctx;
  (শূন্য)data;
  যদি(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

স্থির CURLcode cf_capsule_connect(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   bool *done)
{
  যদি(cf->connected) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }
  যদি(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    যদি(!result && *done)
      cf->connected = TRUE;
    ঘূৰাই result;
  }
  *done = FALSE;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cf_capsule_flush(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data)
{
  সংরচনা cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  যদি(Curl_bufq_is_empty(&ctx->sendbuf))
    ঘূৰাই CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, রিক্ত, 0,
                             &nwritten);
  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ঘূৰাই result;
  }
  ঘূৰাই Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

স্থির CURLcode cf_capsule_send(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                ধ্রুৱ uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  সংরচনা cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (শূন্য)eos;
  *pnwritten = 0;

  যদি(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    যদি(result)
      ঘূৰাই result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  যদি(result)
    ঘূৰাই result;

  result = cf_capsule_flush(cf, data);
  যদি(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  যদি(!result)
    *pnwritten = len;
  ঘূৰাই result;
}

স্থির CURLcode cf_capsule_recv(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                ৱর্ণ *buf, size_t len,
                                size_t *pnread)
{
  সংরচনা cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  যেতিয়ালৈ(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    যদি(result == CURLE_AGAIN)
      ভঙ্গ;
    যদি(result)
      ঘূৰাই result;
    যদি(!nread)
      ভঙ্গ;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned ৱর্ণ *)buf, len,
                                         &result);
  ঘূৰাই result;
}

স্থির bool cf_capsule_data_pending(সংরচনা Curl_cfilter *cf,
                                    ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা cf_capsule_ctx *ctx = cf->ctx;

  যদি(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    ঘূৰাই TRUE;
  ঘূৰাই cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

স্থির CURLcode cf_capsule_cntrl(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  CURLcode result = CURLE_OK;

  (শূন্য)arg1;
  (শূন্য)arg2;
  ৱিকল্প(event) {
  পক্ষ CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_capsule_query(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_capsule_ctx *ctx = cf->ctx;

  (শূন্য)pres2;
  ৱিকল্প(query) {
  পক্ষ CF_QUERY_NEED_FLUSH: {
    যদি(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ঘূৰাই CURLE_OK;
    }
    ভঙ্গ;
  }
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode cf_capsule_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          সংরচনা easy_pollset *ps)
{
  সংরচনা cf_capsule_ctx *ctx = cf->ctx;

  যদি(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    যদি(sock != CURL_SOCKET_BAD)
      ঘূৰাই Curl_pollset_add_out(data, ps, sock);
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cf_capsule_shutdown(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  যদি(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  অন্যথা {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    যদি(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  ঘূৰাই result;
}

সংরচনা Curl_cftype Curl_cft_capsule = {
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

স্থির CURLcode cf_capsule_create(সংরচনা Curl_cfilter **pcf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_capsule_ctx *ctx;
  CURLcode result;

  (শূন্য)data;
  (শূন্য)conn;
  *pcf = রিক্ত;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : রিক্ত;
  যদি(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ঘূৰাই result;
}

CURLcode Curl_cf_capsule_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                      সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  যদি(!result)
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  ঘূৰাই result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
