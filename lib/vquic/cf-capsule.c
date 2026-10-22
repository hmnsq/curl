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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_trc.h"
#ضمن "bufq.h"
#ضمن "select.h"
#ضمن "vquic/capsule.h"
#ضمن "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#وصف_ڪريو CAPSULE_RECV_CHUNKS    4
#وصف_ڪريو CAPSULE_SEND_CHUNKS    4
#وصف_ڪريو CAPSULE_CHUNK_سائيز     (16 * 1024)

بنية cf_capsule_ctx {
  بنية bufq recvbuf;
  بنية bufq sendbuf;
};

ساكن فراغ cf_capsule_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  (فراغ)معلومات;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ساكن CURLcode cf_capsule_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   bool *پورو)
{
  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }
  إذا(cf->ايندڙ) {
    CURLcode نتيجو = cf->ايندڙ->cft->do_connect(cf->ايندڙ, معلومات, پورو);
    إذا(!نتيجو && *پورو)
      cf->connected = TRUE;
    أرجع نتيجو;
  }
  *پورو = FALSE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_flush(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t nwritten;

  إذا(Curl_bufq_is_خالي_جايون(&ctx->sendbuf))
    أرجع CURLE_OK;

  نتيجو = Curl_cf_send_bufq(cf->ايندڙ, معلومات, &ctx->sendbuf, عدم, 0,
                             &nwritten);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    أرجع نتيجو;
  }
  أرجع Curl_bufq_is_خالي_جايون(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ساكن CURLcode cf_capsule_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                ثابت uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتيجو;

  (فراغ)eos;
  *pnwritten = 0;

  إذا(Curl_bufq_is_ڀريل(&ctx->sendbuf)) {
    نتيجو = cf_capsule_flush(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* encapsulate new payload into a capsule */
  نتيجو = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = cf_capsule_flush(cf, معلومات);
  إذا(نتيجو == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    نتيجو = CURLE_OK;
  }

  إذا(!نتيجو)
    *pnwritten = len;
  أرجع نتيجو;
}

ساكن CURLcode cf_capsule_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتيجو;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  طالما(!Curl_bufq_is_ڀريل(&ctx->recvbuf)) {
    نتيجو = Curl_cf_recv_bufq(cf->ايندڙ, معلومات, &ctx->recvbuf, 0, &nread);
    إذا(نتيجو == CURLE_AGAIN)
      اكسر;
    إذا(نتيجو)
      أرجع نتيجو;
    إذا(!nread)
      اكسر;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, معلومات, &ctx->recvbuf,
                                         (بنانشان حرف *)buf, len,
                                         &نتيجو);
  أرجع نتيجو;
}

ساكن bool cf_capsule_معلومات_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *معلومات)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_خالي_جايون(&ctx->recvbuf))
    أرجع TRUE;
  أرجع cf->ايندڙ ? cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات) : FALSE;
}

ساكن CURLcode cf_capsule_cntrl(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتيجو = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_FLUSH:
    نتيجو = cf_capsule_flush(cf, معلومات);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_capsule_query(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  (فراغ)pres2;
  بدل(query) {
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_خالي_جايون(&ctx->sendbuf)) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_capsule_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          بنية easy_pollset *ps)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_خالي_جايون(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, معلومات);
    إذا(sock != CURL_SOCKET_BAD)
      أرجع Curl_pollset_جوڙيو_ٻاهريون(معلومات, ps, sock);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_shutdown(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;

  إذا(!cf->connected || cf->shutdown) {
    *پورو = TRUE;
  }
  وإلا {
    نتيجو = cf_capsule_flush(cf, معلومات);
    *پورو = !نتيجو;
    إذا(نتيجو == CURLE_AGAIN)
      نتيجو = CURLE_OK;
  }
  أرجع نتيجو;
}

بنية Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_معلومات_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

ساكن CURLcode cf_capsule_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_capsule_ctx *ctx;
  CURLcode نتيجو;

  (فراغ)معلومات;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_سائيز, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندي);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_سائيز, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندي);

  نتيجو = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

ٻاهريون:
  *pcf = (!نتيجو) ? cf : عدم;
  إذا(نتيجو && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  أرجع نتيجو;
}

CURLcode Curl_cf_capsule_وجهو_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;

  نتيجو = cf_capsule_create(&cf, معلومات, cf_at->conn);
  إذا(!نتيجو)
    Curl_conn_cf_وجهو_after(cf_at, cf);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
