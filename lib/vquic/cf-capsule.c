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
#تعریف_کرو CAPSULE_RECV_CHUNKS    4
#تعریف_کرو CAPSULE_SEND_CHUNKS    4
#تعریف_کرو CAPSULE_CHUNK_ناپ     (16 * 1024)

بنية cf_capsule_ctx {
  بنية bufq recvbuf;
  بنية bufq sendbuf;
};

ساكن فراغ cf_capsule_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  (فراغ)انکڑے;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ساكن CURLcode cf_capsule_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   bool *پورا)
{
  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }
  إذا(cf->اگلا) {
    CURLcode نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, انکڑے, پورا);
    إذا(!نتیجہ && *پورا)
      cf->connected = TRUE;
    أرجع نتیجہ;
  }
  *پورا = FALSE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_flush(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nwritten;

  إذا(Curl_bufq_is_خالی_تھاواں(&ctx->sendbuf))
    أرجع CURLE_OK;

  نتیجہ = Curl_cf_send_bufq(cf->اگلا, انکڑے, &ctx->sendbuf, عدم, 0,
                             &nwritten);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(انکڑے, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    أرجع نتیجہ;
  }
  أرجع Curl_bufq_is_خالی_تھاواں(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ساكن CURLcode cf_capsule_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                ثابت uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  (فراغ)eos;
  *pnwritten = 0;

  إذا(Curl_bufq_is_بھریا(&ctx->sendbuf)) {
    نتیجہ = cf_capsule_flush(cf, انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  /* encapsulate new payload into a capsule */
  نتیجہ = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = cf_capsule_flush(cf, انکڑے);
  إذا(نتیجہ == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    نتیجہ = CURLE_OK;
  }

  إذا(!نتیجہ)
    *pnwritten = len;
  أرجع نتیجہ;
}

ساكن CURLcode cf_capsule_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  طالما(!Curl_bufq_is_بھریا(&ctx->recvbuf)) {
    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, انکڑے, &ctx->recvbuf, 0, &nread);
    إذا(نتیجہ == CURLE_AGAIN)
      اكسر;
    إذا(نتیجہ)
      أرجع نتیجہ;
    إذا(!nread)
      اكسر;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, انکڑے, &ctx->recvbuf,
                                         (بنانشانی حرف *)buf, len,
                                         &نتیجہ);
  أرجع نتیجہ;
}

ساكن bool cf_capsule_انکڑے_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *انکڑے)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_خالی_تھاواں(&ctx->recvbuf))
    أرجع TRUE;
  أرجع cf->اگلا ? cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے) : FALSE;
}

ساكن CURLcode cf_capsule_cntrl(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_FLUSH:
    نتیجہ = cf_capsule_flush(cf, انکڑے);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_capsule_query(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  (فراغ)pres2;
  بدل(query) {
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->sendbuf)) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_capsule_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *انکڑے,
                                          بنية easy_pollset *ps)
{
  بنية cf_capsule_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, انکڑے);
    إذا(sock != CURL_SOCKET_BAD)
      أرجع Curl_pollset_جوڑو_باہرلا(انکڑے, ps, sock);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_capsule_shutdown(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(!cf->connected || cf->shutdown) {
    *پورا = TRUE;
  }
  وإلا {
    نتیجہ = cf_capsule_flush(cf, انکڑے);
    *پورا = !نتیجہ;
    إذا(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
  }
  أرجع نتیجہ;
}

بنية Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_انکڑے_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

ساكن CURLcode cf_capsule_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *انکڑے,
                                  بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_capsule_ctx *ctx;
  CURLcode نتیجہ;

  (فراغ)انکڑے;
  (فراغ)conn;
  *pcf = عدم;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_ناپ, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_ناپ, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

باہرلا:
  *pcf = (!نتیجہ) ? cf : عدم;
  إذا(نتیجہ && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  أرجع نتیجہ;
}

CURLcode Curl_cf_capsule_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *انکڑے)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_capsule_create(&cf, انکڑے, cf_at->conn);
  إذا(!نتیجہ)
    Curl_conn_cf_داخل_کرو_after(cf_at, cf);
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
