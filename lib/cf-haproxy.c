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

#اگر_تعریف_نشده CURL_DISABLE_PROXY

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "cf-haproxy.h"
#ضمن "connect.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "select.h"


تعريفنوع تعداد {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_انجامشده      /* all work done */
} haproxy_state;

بنية cf_haproxy_ctx {
  صحيح state;
  بنية dynbuf داده_بیرون;
};

ساكن فراغ cf_haproxy_ctx_free(بنية cf_haproxy_ctx *ctx)
{
  إذا(ctx) {
    curlx_dyn_free(&ctx->داده_بیرون);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_haproxy_date_بیرون_مجموعه(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ثابت حرف *client_منبع_ip;
  ثابت حرف *client_dest_ip;
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#اگر_تعریف_شده USE_UNIX_SOCKETS
  إذا(Curl_conn_get_اولی_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    نتیجه = curlx_dyn_addn(&ctx->داده_بیرون, STRCONST("PROXY UNKNOWN\r\n"));
  وإلا {
#پایان_اگر /* USE_UNIX_SOCKETS */
  نتیجه = Curl_conn_cf_get_ip_info(cf->بعدی, داده, &is_ipv6, &ipquad);
  إذا(نتیجه)
    أرجع نتیجه;

  client_منبع_ip = CURL_EASY_STR(داده, STRING_HAPROXY_CLIENT_IP);
  إذا(client_منبع_ip) {
    client_dest_ip = client_منبع_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_منبع_ip);
  }
  وإلا {
    client_منبع_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  نتیجه = curlx_dyn_addf(&ctx->داده_بیرون, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_منبع_ip, client_dest_ip,
                          ipquad.local_پورت, ipquad.remote_پورت);

#اگر_تعریف_شده USE_UNIX_SOCKETS
  }
#پایان_اگر /* USE_UNIX_SOCKETS */
  أرجع نتیجه;
}

ساكن CURLcode cf_haproxy_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   bool *انجامشده)
{
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه;
  size_t len;

  DEBUGASSERT(ctx);
  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  نتیجه = cf->بعدی->cft->do_connect(cf->بعدی, داده, انجامشده);
  إذا(نتیجه || !*انجامشده)
    أرجع نتیجه;

  بدل(ctx->state) {
  حالة HAPROXY_INIT:
    نتیجه = cf_haproxy_date_بیرون_مجموعه(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  حالة HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->داده_بیرون);
    إذا(len > 0) {
      size_t nwritten;
      نتیجه = Curl_conn_cf_send(cf->بعدی, داده,
                                 curlx_dyn_uptr(&ctx->داده_بیرون), len, FALSE,
                                 &nwritten);
      إذا(نتیجه) {
        إذا(نتیجه != CURLE_AGAIN)
          اذهب بیرون;
        نتیجه = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_دم(&ctx->داده_بیرون, len - nwritten);
      إذا(curlx_dyn_len(&ctx->داده_بیرون) > 0) {
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
    }
    ctx->state = HAPROXY_انجامشده;
    FALLTHROUGH();
  افتراضي:
    curlx_dyn_free(&ctx->داده_بیرون);
    اكسر;
  }

بیرون:
  *انجامشده = (!نتیجه) && (ctx->state == HAPROXY_انجامشده);
  cf->connected = *انجامشده;
  أرجع نتیجه;
}

ساكن فراغ cf_haproxy_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده)
{
  CURL_TRC_CF(داده, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ساكن CURLcode cf_haproxy_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده,
                                          بنية easy_pollset *ps)
{
  إذا(cf->بعدی->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    أرجع Curl_pollset_مجموعه_بیرون_only(
      داده, ps, Curl_conn_cf_get_socket(cf, داده));
  }
  أرجع CURLE_OK;
}

بنية Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_راهاندازی,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_داده_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_haproxy_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_haproxy_ctx *ctx;
  CURLcode نتیجه;

  (فراغ)داده;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->داده_بیرون, DYN_HAXPROXY);

  نتیجه = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  ctx = عدم;

بیرون:
  cf_haproxy_ctx_free(ctx);
  *pcf = نتیجه ? عدم : cf;
  أرجع نتیجه;
}

CURLcode Curl_cf_haproxy_درج_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;

  نتیجه = cf_haproxy_create(&cf, داده);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_درج_after(cf_at, cf);

بیرون:
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_PROXY */
