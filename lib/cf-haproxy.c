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

#که_نه_تعریف_شوی CURL_DISABLE_PROXY

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
  HAPROXY_بشپړ      /* all work done */
} haproxy_state;

بنية cf_haproxy_ctx {
  صحيح state;
  بنية dynbuf معلومات_بهر;
};

ساكن فراغ cf_haproxy_ctx_free(بنية cf_haproxy_ctx *ctx)
{
  إذا(ctx) {
    curlx_dyn_free(&ctx->معلومات_بهر);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_haproxy_date_بهر_سېټ(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ثابت حرف *client_سرچینه_ip;
  ثابت حرف *client_dest_ip;
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode پایله;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#که_تعریف_شوی USE_UNIX_SOCKETS
  إذا(Curl_conn_get_لومړی_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    پایله = curlx_dyn_addn(&ctx->معلومات_بهر, STRCONST("PROXY UNKNOWN\r\n"));
  وإلا {
#د_که_پای /* USE_UNIX_SOCKETS */
  پایله = Curl_conn_cf_get_ip_info(cf->بل, معلومات, &is_ipv6, &ipquad);
  إذا(پایله)
    أرجع پایله;

  client_سرچینه_ip = CURL_EASY_STR(معلومات, STRING_HAPROXY_CLIENT_IP);
  إذا(client_سرچینه_ip) {
    client_dest_ip = client_سرچینه_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_سرچینه_ip);
  }
  وإلا {
    client_سرچینه_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  پایله = curlx_dyn_addf(&ctx->معلومات_بهر, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_سرچینه_ip, client_dest_ip,
                          ipquad.local_پورټ, ipquad.remote_پورټ);

#که_تعریف_شوی USE_UNIX_SOCKETS
  }
#د_که_پای /* USE_UNIX_SOCKETS */
  أرجع پایله;
}

ساكن CURLcode cf_haproxy_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   bool *بشپړ)
{
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode پایله;
  size_t len;

  DEBUGASSERT(ctx);
  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  پایله = cf->بل->cft->do_connect(cf->بل, معلومات, بشپړ);
  إذا(پایله || !*بشپړ)
    أرجع پایله;

  بدل(ctx->state) {
  حالة HAPROXY_INIT:
    پایله = cf_haproxy_date_بهر_سېټ(cf, معلومات);
    إذا(پایله)
      اذهب بهر;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  حالة HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->معلومات_بهر);
    إذا(len > 0) {
      size_t nwritten;
      پایله = Curl_conn_cf_send(cf->بل, معلومات,
                                 curlx_dyn_uptr(&ctx->معلومات_بهر), len, FALSE,
                                 &nwritten);
      إذا(پایله) {
        إذا(پایله != CURLE_AGAIN)
          اذهب بهر;
        پایله = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_لکۍ(&ctx->معلومات_بهر, len - nwritten);
      إذا(curlx_dyn_len(&ctx->معلومات_بهر) > 0) {
        پایله = CURLE_OK;
        اذهب بهر;
      }
    }
    ctx->state = HAPROXY_بشپړ;
    FALLTHROUGH();
  افتراضي:
    curlx_dyn_free(&ctx->معلومات_بهر);
    اكسر;
  }

بهر:
  *بشپړ = (!پایله) && (ctx->state == HAPROXY_بشپړ);
  cf->connected = *بشپړ;
  أرجع پایله;
}

ساكن فراغ cf_haproxy_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات)
{
  CURL_TRC_CF(معلومات, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ساكن CURLcode cf_haproxy_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          بنية easy_pollset *ps)
{
  إذا(cf->بل->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    أرجع Curl_pollset_سېټ_بهر_only(
      معلومات, ps, Curl_conn_cf_get_socket(cf, معلومات));
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
  Curl_cf_def_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_haproxy_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_haproxy_ctx *ctx;
  CURLcode پایله;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->معلومات_بهر, DYN_HAXPROXY);

  پایله = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  إذا(پایله)
    اذهب بهر;
  ctx = عدم;

بهر:
  cf_haproxy_ctx_free(ctx);
  *pcf = پایله ? عدم : cf;
  أرجع پایله;
}

CURLcode Curl_cf_haproxy_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;

  پایله = cf_haproxy_create(&cf, معلومات);
  إذا(پایله)
    اذهب بهر;
  Curl_conn_cf_ورننباسه_after(cf_at, cf);

بهر:
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_PROXY */
