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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-haproxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_ਪੂਰਾ      /* all work done */
} haproxy_state;

ਬਣਤਰ cf_haproxy_ctx {
  ਪੂਰਨ state;
  ਬਣਤਰ dynbuf ਅੰਕੜੇ_ਬਾਹਰਲਾ;
};

ਸਥਿਰ ਖਾਲੀ cf_haproxy_ctx_free(ਬਣਤਰ cf_haproxy_ctx *ctx)
{
  ਜੇ(ctx) {
    curlx_dyn_free(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ);
    curlx_free(ctx);
  }
}

ਸਥਿਰ CURLcode cf_haproxy_date_ਬਾਹਰਲਾ_ਸੈੱਟ(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ਅਚਲ ਅੱਖਰ *client_ਸਰੋਤ_ip;
  ਅਚਲ ਅੱਖਰ *client_dest_ip;
  ਬਣਤਰ cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  ਜੇ(Curl_conn_get_ਪਹਿਲਾ_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    ਨਤੀਜਾ = curlx_dyn_addn(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ, STRCONST("PROXY UNKNOWN\r\n"));
  ਨਹੀਂਤਾਂ {
#ਜੇ_ਅੰਤ /* USE_UNIX_SOCKETS */
  ਨਤੀਜਾ = Curl_conn_cf_get_ip_info(cf->ਅਗਲਾ, ਅੰਕੜੇ, &is_ipv6, &ipquad);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  client_ਸਰੋਤ_ip = CURL_EASY_STR(ਅੰਕੜੇ, STRING_HAPROXY_CLIENT_IP);
  ਜੇ(client_ਸਰੋਤ_ip) {
    client_dest_ip = client_ਸਰੋਤ_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_ਸਰੋਤ_ip);
  }
  ਨਹੀਂਤਾਂ {
    client_ਸਰੋਤ_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  ਨਤੀਜਾ = curlx_dyn_addf(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_ਸਰੋਤ_ip, client_dest_ip,
                          ipquad.local_ਪੋਰਟ, ipquad.remote_ਪੋਰਟ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  }
#ਜੇ_ਅੰਤ /* USE_UNIX_SOCKETS */
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_haproxy_connect(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;
  size_t len;

  DEBUGASSERT(ctx);
  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਚੋਣ(ctx->state) {
  ਹਾਲਤ HAPROXY_INIT:
    ਨਤੀਜਾ = cf_haproxy_date_ਬਾਹਰਲਾ_ਸੈੱਟ(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  ਹਾਲਤ HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ);
    ਜੇ(len > 0) {
      size_t nwritten;
      ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                                 curlx_dyn_uptr(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ), len, FALSE,
                                 &nwritten);
      ਜੇ(ਨਤੀਜਾ) {
        ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
          ਜਾਓ ਬਾਹਰਲਾ;
        ਨਤੀਜਾ = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_ਪੂਛ(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ, len - nwritten);
      ਜੇ(curlx_dyn_len(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ) > 0) {
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ctx->state = HAPROXY_ਪੂਰਾ;
    FALLTHROUGH();
  ਮੂਲ:
    curlx_dyn_free(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ);
    ਰੋਕੋ;
  }

ਬਾਹਰਲਾ:
  *ਪੂਰਾ = (!ਨਤੀਜਾ) && (ctx->state == HAPROXY_ਪੂਰਾ);
  cf->connected = *ਪੂਰਾ;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_haproxy_destroy(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ਸਥਿਰ CURLcode cf_haproxy_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ easy_pollset *ps)
{
  ਜੇ(cf->ਅਗਲਾ->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    ਵਾਪਸ Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(
      ਅੰਕੜੇ, ps, Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ));
  }
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_ਤਿਆਰੀ,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ਸਥਿਰ CURLcode cf_haproxy_create(ਬਣਤਰ Curl_cfilter **pcf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_haproxy_ctx *ctx;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->ਅੰਕੜੇ_ਬਾਹਰਲਾ, DYN_HAXPROXY);

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = ਨਿਰਾ;

ਬਾਹਰਲਾ:
  cf_haproxy_ctx_free(ctx);
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_haproxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_haproxy_create(&cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */
