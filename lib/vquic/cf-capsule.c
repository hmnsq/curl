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
#تعریف_کن CAPSULE_RECV_CHUNKS    4
#تعریف_کن CAPSULE_SEND_CHUNKS    4
#تعریف_کن CAPSULE_CHUNK_اندازه     (16 * 1024)

بنية cf_capsule_ctx {
  بنية bufq recvbuf;
  بنية bufq sendbuf;
};

ساكن فراغ cf_capsule_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  (فراغ)داده;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ساكن CURLcode cf_capsule_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   bool *انجامشده)
{
  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }
  إذا(cf->بعدی) {
    CURLcode نتیجه = cf->بعدی->cft->do_connect(cf->بعدی, داده, انجامشده);
    إذا(!نتیجه && *انجامشده)
      cf->connected = TRUE;
    أرجع نتیجه;
  }
  *انجامشده = FALSE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_flush(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t nwritten;

  إذا(Curl_bufq_is_خالی(&ctx->sendbuf))
    أرجع CURLE_OK;

  نتیجه = Curl_cf_send_bufq(cf->بعدی, داده, &ctx->sendbuf, عدم, 0,
                             &nwritten);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN) {
      CURL_TRC_CF(داده, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    أرجع نتیجه;
  }
  أرجع Curl_bufq_is_خالی(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ساكن CURLcode cf_capsule_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                ثابت uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجه;

  (فراغ)eos;
  *pnwritten = 0;

  إذا(Curl_bufq_is_پر(&ctx->sendbuf)) {
    نتیجه = cf_capsule_flush(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* encapsulate new payload into a capsule */
  نتیجه = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = cf_capsule_flush(cf, داده);
  إذا(نتیجه == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    نتیجه = CURLE_OK;
  }

  إذا(!نتیجه)
    *pnwritten = len;
  أرجع نتیجه;
}

ساكن CURLcode cf_capsule_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجه;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  طالما(!Curl_bufq_is_پر(&ctx->recvbuf)) {
    نتیجه = Curl_cf_recv_bufq(cf->بعدی, داده, &ctx->recvbuf, 0, &nread);
    إذا(نتیجه == CURLE_AGAIN)
      اكسر;
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(!nread)
      اكسر;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, داده, &ctx->recvbuf,
                                         (بیعلامت حرف *)buf, len,
                                         &نتیجه);
  أرجع نتیجه;
}

ساكن bool cf_capsule_داده_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *داده)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_خالی(&ctx->recvbuf))
    أرجع TRUE;
  أرجع cf->بعدی ? cf->بعدی->cft->has_داده_pending(cf->بعدی, داده) : FALSE;
}

ساكن CURLcode cf_capsule_cntrl(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجه = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_FLUSH:
    نتیجه = cf_capsule_flush(cf, داده);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_capsule_query(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  (فراغ)pres2;
  بدل(query) {
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_خالی(&ctx->sendbuf)) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_capsule_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده,
                                          بنية easy_pollset *ps)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_خالی(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, داده);
    إذا(sock != CURL_SOCKET_BAD)
      أرجع Curl_pollset_جمع_کن_بیرون(داده, ps, sock);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_shutdown(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(!cf->connected || cf->shutdown) {
    *انجامشده = TRUE;
  }
  وإلا {
    نتیجه = cf_capsule_flush(cf, داده);
    *انجامشده = !نتیجه;
    إذا(نتیجه == CURLE_AGAIN)
      نتیجه = CURLE_OK;
  }
  أرجع نتیجه;
}

بنية Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_داده_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

ساكن CURLcode cf_capsule_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *داده,
                                  بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_capsule_ctx *ctx;
  CURLcode نتیجه;

  (فراغ)داده;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_اندازه, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_اندازه, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد);

  نتیجه = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

بیرون:
  *pcf = (!نتیجه) ? cf : عدم;
  إذا(نتیجه && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  أرجع نتیجه;
}

CURLcode Curl_cf_capsule_درج_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;

  نتیجه = cf_capsule_create(&cf, داده, cf_at->conn);
  إذا(!نتیجه)
    Curl_conn_cf_درج_after(cf_at, cf);
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
