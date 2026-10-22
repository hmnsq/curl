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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY

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
  HAPROXY_پورو      /* all work done */
} haproxy_state;

بنية cf_haproxy_ctx {
  صحيح state;
  بنية dynbuf معلومات_ٻاهريون;
};

ساكن فراغ cf_haproxy_ctx_free(بنية cf_haproxy_ctx *ctx)
{
  إذا(ctx) {
    curlx_dyn_free(&ctx->معلومات_ٻاهريون);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_haproxy_date_ٻاهريون_سيٽ(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ثابت حرف *client_ذريعو_ip;
  ثابت حرف *client_dest_ip;
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
  إذا(Curl_conn_get_پهريون_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    نتيجو = curlx_dyn_addn(&ctx->معلومات_ٻاهريون, STRCONST("PROXY UNKNOWN\r\n"));
  وإلا {
#جيڪڏهن_پورو /* USE_UNIX_SOCKETS */
  نتيجو = Curl_conn_cf_get_ip_info(cf->ايندڙ, معلومات, &is_ipv6, &ipquad);
  إذا(نتيجو)
    أرجع نتيجو;

  client_ذريعو_ip = CURL_EASY_STR(معلومات, STRING_HAPROXY_CLIENT_IP);
  إذا(client_ذريعو_ip) {
    client_dest_ip = client_ذريعو_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_ذريعو_ip);
  }
  وإلا {
    client_ذريعو_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  نتيجو = curlx_dyn_addf(&ctx->معلومات_ٻاهريون, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_ذريعو_ip, client_dest_ip,
                          ipquad.local_پورٽ, ipquad.remote_پورٽ);

#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
  }
#جيڪڏهن_پورو /* USE_UNIX_SOCKETS */
  أرجع نتيجو;
}

ساكن CURLcode cf_haproxy_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   bool *پورو)
{
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو;
  size_t len;

  DEBUGASSERT(ctx);
  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  نتيجو = cf->ايندڙ->cft->do_connect(cf->ايندڙ, معلومات, پورو);
  إذا(نتيجو || !*پورو)
    أرجع نتيجو;

  بدل(ctx->state) {
  حالة HAPROXY_INIT:
    نتيجو = cf_haproxy_date_ٻاهريون_سيٽ(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  حالة HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->معلومات_ٻاهريون);
    إذا(len > 0) {
      size_t nwritten;
      نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات,
                                 curlx_dyn_uptr(&ctx->معلومات_ٻاهريون), len, FALSE,
                                 &nwritten);
      إذا(نتيجو) {
        إذا(نتيجو != CURLE_AGAIN)
          اذهب ٻاهريون;
        نتيجو = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_پڇ(&ctx->معلومات_ٻاهريون, len - nwritten);
      إذا(curlx_dyn_len(&ctx->معلومات_ٻاهريون) > 0) {
        نتيجو = CURLE_OK;
        اذهب ٻاهريون;
      }
    }
    ctx->state = HAPROXY_پورو;
    FALLTHROUGH();
  افتراضي:
    curlx_dyn_free(&ctx->معلومات_ٻاهريون);
    اكسر;
  }

ٻاهريون:
  *پورو = (!نتيجو) && (ctx->state == HAPROXY_پورو);
  cf->connected = *پورو;
  أرجع نتيجو;
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
  إذا(cf->ايندڙ->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    أرجع Curl_pollset_سيٽ_ٻاهريون_only(
      معلومات, ps, Curl_conn_cf_get_socket(cf, معلومات));
  }
  أرجع CURLE_OK;
}

بنية Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_تياري,
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
  CURLcode نتيجو;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->معلومات_ٻاهريون, DYN_HAXPROXY);

  نتيجو = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;
  ctx = عدم;

ٻاهريون:
  cf_haproxy_ctx_free(ctx);
  *pcf = نتيجو ? عدم : cf;
  أرجع نتيجو;
}

CURLcode Curl_cf_haproxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;

  نتيجو = cf_haproxy_create(&cf, معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;
  Curl_conn_cf_وجهو_after(cf_at, cf);

ٻاهريون:
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY */
