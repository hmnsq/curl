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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "cf-haproxy.h"
#شامل "connect.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "select.h"


قسمتعریف شمار {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_مکمل      /* all work done */
} haproxy_state;

ساخت cf_haproxy_ctx {
  صحیح state;
  ساخت dynbuf ڈیٹا_برآمد;
};

ساکن خالی cf_haproxy_ctx_free(ساخت cf_haproxy_ctx *ctx)
{
  اگر(ctx) {
    curlx_dyn_free(&ctx->ڈیٹا_برآمد);
    curlx_free(ctx);
  }
}

ساکن CURLcode cf_haproxy_date_برآمد_سیٹ(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ثابت حرف *client_ماخذ_ip;
  ثابت حرف *client_dest_ip;
  ساخت cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  ساخت ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  اگر(Curl_conn_get_پہلا_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    نتیجہ = curlx_dyn_addn(&ctx->ڈیٹا_برآمد, STRCONST("PROXY UNKNOWN\r\n"));
  ورنہ {
#اگر_ختم /* USE_UNIX_SOCKETS */
  نتیجہ = Curl_conn_cf_get_ip_info(cf->اگلا, ڈیٹا, &is_ipv6, &ipquad);
  اگر(نتیجہ)
    واپس نتیجہ;

  client_ماخذ_ip = CURL_EASY_STR(ڈیٹا, STRING_HAPROXY_CLIENT_IP);
  اگر(client_ماخذ_ip) {
    client_dest_ip = client_ماخذ_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_ماخذ_ip);
  }
  ورنہ {
    client_ماخذ_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  نتیجہ = curlx_dyn_addf(&ctx->ڈیٹا_برآمد, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_ماخذ_ip, client_dest_ip,
                          ipquad.local_پورٹ, ipquad.remote_پورٹ);

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  }
#اگر_ختم /* USE_UNIX_SOCKETS */
  واپس نتیجہ;
}

ساکن CURLcode cf_haproxy_connect(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   bool *مکمل)
{
  ساخت cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  size_t len;

  DEBUGASSERT(ctx);
  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, ڈیٹا, مکمل);
  اگر(نتیجہ || !*مکمل)
    واپس نتیجہ;

  انتخاب(ctx->state) {
  حالت HAPROXY_INIT:
    نتیجہ = cf_haproxy_date_برآمد_سیٹ(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  حالت HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->ڈیٹا_برآمد);
    اگر(len > 0) {
      size_t nwritten;
      نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                                 curlx_dyn_uptr(&ctx->ڈیٹا_برآمد), len, FALSE,
                                 &nwritten);
      اگر(نتیجہ) {
        اگر(نتیجہ != CURLE_AGAIN)
          جاؤ برآمد;
        نتیجہ = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_دم(&ctx->ڈیٹا_برآمد, len - nwritten);
      اگر(curlx_dyn_len(&ctx->ڈیٹا_برآمد) > 0) {
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
    }
    ctx->state = HAPROXY_مکمل;
    FALLTHROUGH();
  طےشدہ:
    curlx_dyn_free(&ctx->ڈیٹا_برآمد);
    توڑو;
  }

برآمد:
  *مکمل = (!نتیجہ) && (ctx->state == HAPROXY_مکمل);
  cf->connected = *مکمل;
  واپس نتیجہ;
}

ساکن خالی cf_haproxy_destroy(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا)
{
  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ساکن CURLcode cf_haproxy_adjust_pollset(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          ساخت easy_pollset *ps)
{
  اگر(cf->اگلا->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    واپس Curl_pollset_سیٹ_برآمد_only(
      ڈیٹا, ps, Curl_conn_cf_get_socket(cf, ڈیٹا));
  }
  واپس CURLE_OK;
}

ساخت Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_تیاری,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساکن CURLcode cf_haproxy_create(ساخت Curl_cfilter **pcf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_haproxy_ctx *ctx;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->ڈیٹا_برآمد, DYN_HAXPROXY);

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  ctx = تہی;

برآمد:
  cf_haproxy_ctx_free(ctx);
  *pcf = نتیجہ ? تہی : cf;
  واپس نتیجہ;
}

CURLcode Curl_cf_haproxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                      ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_haproxy_create(&cf, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

برآمد:
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_PROXY */
