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

#جے_بنا_تعریف CURL_DISABLE_PROXY

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
  HAPROXY_پورا      /* all work done */
} haproxy_state;

بنية cf_haproxy_ctx {
  صحيح state;
  بنية dynbuf انکڑے_باہرلا;
};

ساكن فراغ cf_haproxy_ctx_free(بنية cf_haproxy_ctx *ctx)
{
  إذا(ctx) {
    curlx_dyn_free(&ctx->انکڑے_باہرلا);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_haproxy_date_باہرلا_سیٹ(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ثابت حرف *client_سروت_ip;
  ثابت حرف *client_dest_ip;
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#جے_تعریف_شدہ USE_UNIX_SOCKETS
  إذا(Curl_conn_get_پہلا_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    نتیجہ = curlx_dyn_addn(&ctx->انکڑے_باہرلا, STRCONST("PROXY UNKNOWN\r\n"));
  وإلا {
#جے_انت /* USE_UNIX_SOCKETS */
  نتیجہ = Curl_conn_cf_get_ip_info(cf->اگلا, انکڑے, &is_ipv6, &ipquad);
  إذا(نتیجہ)
    أرجع نتیجہ;

  client_سروت_ip = CURL_EASY_STR(انکڑے, STRING_HAPROXY_CLIENT_IP);
  إذا(client_سروت_ip) {
    client_dest_ip = client_سروت_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_سروت_ip);
  }
  وإلا {
    client_سروت_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  نتیجہ = curlx_dyn_addf(&ctx->انکڑے_باہرلا, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_سروت_ip, client_dest_ip,
                          ipquad.local_پورٹ, ipquad.remote_پورٹ);

#جے_تعریف_شدہ USE_UNIX_SOCKETS
  }
#جے_انت /* USE_UNIX_SOCKETS */
  أرجع نتیجہ;
}

ساكن CURLcode cf_haproxy_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   bool *پورا)
{
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  size_t len;

  DEBUGASSERT(ctx);
  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, انکڑے, پورا);
  إذا(نتیجہ || !*پورا)
    أرجع نتیجہ;

  بدل(ctx->state) {
  حالة HAPROXY_INIT:
    نتیجہ = cf_haproxy_date_باہرلا_سیٹ(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  حالة HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->انکڑے_باہرلا);
    إذا(len > 0) {
      size_t nwritten;
      نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے,
                                 curlx_dyn_uptr(&ctx->انکڑے_باہرلا), len, FALSE,
                                 &nwritten);
      إذا(نتیجہ) {
        إذا(نتیجہ != CURLE_AGAIN)
          اذهب باہرلا;
        نتیجہ = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_پوچھ(&ctx->انکڑے_باہرلا, len - nwritten);
      إذا(curlx_dyn_len(&ctx->انکڑے_باہرلا) > 0) {
        نتیجہ = CURLE_OK;
        اذهب باہرلا;
      }
    }
    ctx->state = HAPROXY_پورا;
    FALLTHROUGH();
  افتراضي:
    curlx_dyn_free(&ctx->انکڑے_باہرلا);
    اكسر;
  }

باہرلا:
  *پورا = (!نتیجہ) && (ctx->state == HAPROXY_پورا);
  cf->connected = *پورا;
  أرجع نتیجہ;
}

ساكن فراغ cf_haproxy_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے)
{
  CURL_TRC_CF(انکڑے, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ساكن CURLcode cf_haproxy_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *انکڑے,
                                          بنية easy_pollset *ps)
{
  إذا(cf->اگلا->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    أرجع Curl_pollset_سیٹ_باہرلا_only(
      انکڑے, ps, Curl_conn_cf_get_socket(cf, انکڑے));
  }
  أرجع CURLE_OK;
}

بنية Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_تیاری,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_انکڑے_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_haproxy_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *انکڑے)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_haproxy_ctx *ctx;
  CURLcode نتیجہ;

  (فراغ)انکڑے;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->انکڑے_باہرلا, DYN_HAXPROXY);

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  ctx = عدم;

باہرلا:
  cf_haproxy_ctx_free(ctx);
  *pcf = نتیجہ ? عدم : cf;
  أرجع نتیجہ;
}

CURLcode Curl_cf_haproxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *انکڑے)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_haproxy_create(&cf, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

باہرلا:
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_PROXY */
