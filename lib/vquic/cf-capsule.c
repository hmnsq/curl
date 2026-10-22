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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_trc.h"
#شامل "bufq.h"
#شامل "select.h"
#شامل "vquic/capsule.h"
#شامل "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#تعریف_کرو CAPSULE_RECV_CHUNKS    4
#تعریف_کرو CAPSULE_SEND_CHUNKS    4
#تعریف_کرو CAPSULE_CHUNK_سائز     (16 * 1024)

ساخت cf_capsule_ctx {
  ساخت bufq recvbuf;
  ساخت bufq sendbuf;
};

ساکن خالی cf_capsule_destroy(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_capsule_ctx *ctx = cf->ctx;
  (خالی)ڈیٹا;
  اگر(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ساکن CURLcode cf_capsule_connect(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   bool *مکمل)
{
  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }
  اگر(cf->اگلا) {
    CURLcode نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(!نتیجہ && *مکمل)
      cf->connected = TRUE;
    واپس نتیجہ;
  }
  *مکمل = FALSE;
  واپس CURLE_OK;
}

ساکن CURLcode cf_capsule_flush(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nwritten;

  اگر(Curl_bufq_is_خالی_خانے(&ctx->sendbuf))
    واپس CURLE_OK;

  نتیجہ = Curl_cf_send_bufq(cf->اگلا, ڈیٹا, &ctx->sendbuf, تہی, 0,
                             &nwritten);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(ڈیٹا, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    واپس نتیجہ;
  }
  واپس Curl_bufq_is_خالی_خانے(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ساکن CURLcode cf_capsule_send(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ثابت uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  ساخت cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  (خالی)eos;
  *pnwritten = 0;

  اگر(Curl_bufq_is_بھرا(&ctx->sendbuf)) {
    نتیجہ = cf_capsule_flush(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* encapsulate new payload into a capsule */
  نتیجہ = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = cf_capsule_flush(cf, ڈیٹا);
  اگر(نتیجہ == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    نتیجہ = CURLE_OK;
  }

  اگر(!نتیجہ)
    *pnwritten = len;
  واپس نتیجہ;
}

ساکن CURLcode cf_capsule_recv(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  ساخت cf_capsule_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  جب(!Curl_bufq_is_بھرا(&ctx->recvbuf)) {
    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, ڈیٹا, &ctx->recvbuf, 0, &nread);
    اگر(نتیجہ == CURLE_AGAIN)
      توڑو;
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(!nread)
      توڑو;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, ڈیٹا, &ctx->recvbuf,
                                         (غیرعلامتی حرف *)buf, len,
                                         &نتیجہ);
  واپس نتیجہ;
}

ساکن bool cf_capsule_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                    ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_capsule_ctx *ctx = cf->ctx;

  اگر(ctx && !Curl_bufq_is_خالی_خانے(&ctx->recvbuf))
    واپس TRUE;
  واپس cf->اگلا ? cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا) : FALSE;
}

ساکن CURLcode cf_capsule_cntrl(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 صحیح event, صحیح arg1, خالی *arg2)
{
  CURLcode نتیجہ = CURLE_OK;

  (خالی)arg1;
  (خالی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_FLUSH:
    نتیجہ = cf_capsule_flush(cf, ڈیٹا);
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_capsule_query(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_capsule_ctx *ctx = cf->ctx;

  (خالی)pres2;
  انتخاب(query) {
  حالت CF_QUERY_NEED_FLUSH: {
    اگر(!Curl_bufq_is_خالی_خانے(&ctx->sendbuf)) {
      *pres1 = TRUE;
      واپس CURLE_OK;
    }
    توڑو;
  }
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساکن CURLcode cf_capsule_adjust_pollset(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          ساخت easy_pollset *ps)
{
  ساخت cf_capsule_ctx *ctx = cf->ctx;

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ڈیٹا);
    اگر(sock != CURL_SOCKET_BAD)
      واپس Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, sock);
  }
  واپس CURLE_OK;
}

ساکن CURLcode cf_capsule_shutdown(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(!cf->connected || cf->shutdown) {
    *مکمل = TRUE;
  }
  ورنہ {
    نتیجہ = cf_capsule_flush(cf, ڈیٹا);
    *مکمل = !نتیجہ;
    اگر(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
  }
  واپس نتیجہ;
}

ساخت Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_ڈیٹا_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

ساکن CURLcode cf_capsule_create(ساخت Curl_cfilter **pcf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_capsule_ctx *ctx;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  (خالی)conn;
  *pcf = تہی;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_سائز, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_سائز, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

برآمد:
  *pcf = (!نتیجہ) ? cf : تہی;
  اگر(نتیجہ && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  واپس نتیجہ;
}

CURLcode Curl_cf_capsule_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                      ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_capsule_create(&cf, ڈیٹا, cf_at->conn);
  اگر(!نتیجہ)
    Curl_conn_cf_داخل_کرو_after(cf_at, cf);
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
