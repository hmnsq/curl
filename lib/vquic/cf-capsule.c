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
#تعریف_کړه CAPSULE_RECV_CHUNKS    4
#تعریف_کړه CAPSULE_SEND_CHUNKS    4
#تعریف_کړه CAPSULE_CHUNK_اندازه     (16 * 1024)

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
                                   bool *بشپړ)
{
  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }
  إذا(cf->بل) {
    CURLcode پایله = cf->بل->cft->do_connect(cf->بل, معلومات, بشپړ);
    إذا(!پایله && *بشپړ)
      cf->connected = TRUE;
    أرجع پایله;
  }
  *بشپړ = FALSE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_flush(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  size_t nwritten;

  إذا(Curl_bufq_is_تشه(&ctx->sendbuf))
    أرجع CURLE_OK;

  پایله = Curl_cf_send_bufq(cf->بل, معلومات, &ctx->sendbuf, عدم, 0,
                             &nwritten);
  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    أرجع پایله;
  }
  أرجع Curl_bufq_is_تشه(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ساكن CURLcode cf_capsule_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                ثابت uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode پایله;

  (فراغ)eos;
  *pnwritten = 0;

  إذا(Curl_bufq_is_ډک(&ctx->sendbuf)) {
    پایله = cf_capsule_flush(cf, معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  /* encapsulate new payload into a capsule */
  پایله = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  إذا(پایله)
    أرجع پایله;

  پایله = cf_capsule_flush(cf, معلومات);
  إذا(پایله == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    پایله = CURLE_OK;
  }

  إذا(!پایله)
    *pnwritten = len;
  أرجع پایله;
}

ساكن CURLcode cf_capsule_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode پایله;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  طالما(!Curl_bufq_is_ډک(&ctx->recvbuf)) {
    پایله = Curl_cf_recv_bufq(cf->بل, معلومات, &ctx->recvbuf, 0, &nread);
    إذا(پایله == CURLE_AGAIN)
      اكسر;
    إذا(پایله)
      أرجع پایله;
    إذا(!nread)
      اكسر;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, معلومات, &ctx->recvbuf,
                                         (بینښه حرف *)buf, len,
                                         &پایله);
  أرجع پایله;
}

ساكن bool cf_capsule_معلومات_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *معلومات)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_تشه(&ctx->recvbuf))
    أرجع TRUE;
  أرجع cf->بل ? cf->بل->cft->has_معلومات_pending(cf->بل, معلومات) : FALSE;
}

ساكن CURLcode cf_capsule_cntrl(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode پایله = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_FLUSH:
    پایله = cf_capsule_flush(cf, معلومات);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode cf_capsule_query(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  (فراغ)pres2;
  بدل(query) {
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_تشه(&ctx->sendbuf)) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_capsule_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          بنية easy_pollset *ps)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_تشه(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, معلومات);
    إذا(sock != CURL_SOCKET_BAD)
      أرجع Curl_pollset_جمع_کړه_بهر(معلومات, ps, sock);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_shutdown(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله = CURLE_OK;

  إذا(!cf->connected || cf->shutdown) {
    *بشپړ = TRUE;
  }
  وإلا {
    پایله = cf_capsule_flush(cf, معلومات);
    *بشپړ = !پایله;
    إذا(پایله == CURLE_AGAIN)
      پایله = CURLE_OK;
  }
  أرجع پایله;
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
  CURLcode پایله;

  (فراغ)معلومات;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_اندازه, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_اندازه, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد);

  پایله = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

بهر:
  *pcf = (!پایله) ? cf : عدم;
  إذا(پایله && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  أرجع پایله;
}

CURLcode Curl_cf_capsule_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;

  پایله = cf_capsule_create(&cf, معلومات, cf_at->conn);
  إذا(!پایله)
    Curl_conn_cf_ورننباسه_after(cf_at, cf);
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
