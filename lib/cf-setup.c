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

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "cf-haproxy.h"
#شامل "cf-ip-happy.h"
#شامل "cf-setup.h"
#شامل "curl_trc.h"
#شامل "connect.h"
#شامل "http_proxy.h"
#شامل "socks.h"
#شامل "vquic/cf-capsule.h"
#شامل "vquic/vquic.h"
#شامل "vtls/vtls.h"


قسمتعریف شمار {
  CF_تیاری_INIT,
  CF_تیاری_CNNCT_EYEBALLS,
  CF_تیاری_CNNCT_SOCKS,
  CF_تیاری_CNNCT_HTTP_PROXY,
  CF_تیاری_CNNCT_HAPROXY,
  CF_تیاری_CNNCT_SSL,
  CF_تیاری_مکمل
} cf_تیاری_state;

ساخت cf_تیاری_ctx {
  cf_تیاری_state state;
  صحیح ssl_منوال;
  uint8_t transport;
  uint8_t retry_گنتی;
};

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

ساکن CURLcode cf_تیاری_جوڑو_haproxy(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(ctx->state < CF_تیاری_CNNCT_HAPROXY) {
    اگر(ڈیٹا->سیٹ.haproxyprotocol) {
      اگر(ctx->transport == TRNSPRT_QUIC) {
        failf(ڈیٹا, "haproxy protocol does not support QUIC");
        واپس CURLE_UNSUPPORTED_PROTOCOL;
      }
      نتیجہ = Curl_cf_haproxy_داخل_کرو_after(cf, ڈیٹا);
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "adding HAPROXY filter failed -> %d",
                    (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      CURL_TRC_CF(ڈیٹا, cf, "added HAPROXY filter");
    }
    ctx->state = CF_تیاری_CNNCT_HAPROXY;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_تیاری_جوڑو_socks(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  اگر(ctx->state < CF_تیاری_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    ساخت Curl_peer *دوسرا_peer;

    اگر(cf->conn->http_proxy.peer)
      دوسرا_peer = cf->conn->http_proxy.peer;
    ورنہ
      دوسرا_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    اگر(!دوسرا_peer)
      واپس CURLE_FAILED_INIT;

    نتیجہ = Curl_cf_socks_proxy_داخل_کرو_after(
      cf, ڈیٹا, دوسرا_peer, cf->conn->ip_نسخہ,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "adding SOCKS filter failed -> %d", (صحیح)نتیجہ);
      واپس نتیجہ;
    }

    CURL_TRC_CF(ڈیٹا, cf, "added SOCKS filter to %s:%u",
                دوسرا_peer->hostname, دوسرا_peer->پورٹ);
    ctx->state = CF_تیاری_CNNCT_SOCKS;
  }
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
ساکن CURLcode cf_تیاری_جوڑو_http_proxy(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(ctx->state < CF_تیاری_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->بٹ.origin_is_proxy) {
    ساخت Curl_peer *peer = cf->conn->http_proxy.peer;
    ساخت Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#اگر_تعریف_شدہ USE_SSL
    اگر(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      نتیجہ = Curl_cf_ssl_proxy_داخل_کرو_after(
        cf, ڈیٹا, cf->conn->http_proxy.peer);
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      CURL_TRC_CF(ڈیٹا, cf, "added SSL filter for HTTP proxy");
    }
#اگر_ختم /* USE_SSL */

    نتیجہ = Curl_cf_http_proxy_داخل_کرو_after(
      cf, ڈیٹا, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (صحیح)نتیجہ);
      واپس نتیجہ;
    }
    CURL_TRC_CF(ڈیٹا, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_تیاری_CNNCT_HTTP_PROXY;
  }
  واپس نتیجہ;
}
#اگر_ختم /* !CURL_DISABLE_HTTP */
#اگر_ختم /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ساکن ساخت Curl_peer *conn_get_پہلا_origin(ساخت connectdata *conn,
                                             int8_t sockindex)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(conn->socks_proxy.peer)
    واپس conn->socks_proxy.peer;
  اگر(conn->http_proxy.peer)
    واپس conn->http_proxy.peer;
#اگر_ختم
  واپس (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ساکن CURLcode cf_تیاری_جوڑو_ip_happy(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(ctx->state < CF_تیاری_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    ساخت Curl_peer *پہلا_origin =
      conn_get_پہلا_origin(cf->conn, cf->sockindex);
    ساخت Curl_peer *پہلا_peer =
      Curl_conn_get_پہلا_peer(cf->conn, cf->sockindex);
    ساخت Curl_peer *tunnel_peer = تہی;
    uint8_t پہلا_transport = ctx->transport;

    اگر(!پہلا_peer)
      واپس CURLE_FAILED_INIT;

#اگر !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    اگر(cf->conn->http_proxy.peer && !cf->conn->بٹ.origin_is_proxy) {
      پہلا_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      اگر((پہلا_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(ڈیٹا, "HTTP/3 proxy not possible via SOCKS");
        واپس CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#اگر_ختم /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    نتیجہ = Curl_cf_ip_happy_داخل_کرو_after(cf, ڈیٹا, پہلا_origin, پہلا_peer,
                                           پہلا_transport,
                                           tunnel_peer, ctx->transport);
    اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "adding happy eyeballs failed -> %d", (صحیح)نتیجہ);
      واپس نتیجہ;
    }

    اگر(tunnel_peer && (پہلا_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(ڈیٹا, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  پہلا_peer->hostname, پہلا_peer->پورٹ);
      ctx->state = CF_تیاری_CNNCT_HTTP_PROXY;
    }
    ورنہ {
      CURL_TRC_CF(ڈیٹا, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  پہلا_peer->hostname, پہلا_peer->پورٹ);
      ctx->state = CF_تیاری_CNNCT_EYEBALLS;
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_تیاری_جوڑو_origin_filters(ساخت Curl_cfilter *cf,
                                            ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)ڈیٹا; /* not used in all builds */
  اگر(ctx->state < CF_تیاری_CNNCT_SSL) {
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    اگر(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->بٹ.origin_is_proxy) {
      ساخت Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      ساخت Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      نتیجہ = Curl_cf_capsule_داخل_کرو_after(cf, ڈیٹا);
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "adding capsule filter failed -> %d",
                    (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      نتیجہ = Curl_cf_quic_داخل_کرو_after(cf, ڈیٹا, origin, peer);
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "adding QUIC filter failed -> %d", (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      CURL_TRC_CF(ڈیٹا, cf, "added QUIC filter for origin");
    }
    ورنہ
#اگر_ختم /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#اگر_تعریف_شدہ USE_SSL
    اگر((ctx->ssl_منوال == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_منوال != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
      اگر(cf->conn->بٹ.origin_is_proxy) {
        نتیجہ = Curl_cf_ssl_proxy_داخل_کرو_after(cf, ڈیٹا, cf->conn->origin);
      }
      ورنہ
#اگر_ختم
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        نتیجہ = Curl_cf_ssl_داخل_کرو_after(cf, ڈیٹا,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "adding SSL filter for origin failed -> %d",
                    (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      CURL_TRC_CF(ڈیٹا, cf, "added SSL filter for origin");
    }
#اگر_ختم /* USE_SSL */
    ctx->state = CF_تیاری_CNNCT_SSL;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_تیاری_connect_اقدام(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       bool *مکمل)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(ڈیٹا, cf->sockindex, "cf_setup_connect"));

  اگر(cf->اگلا && !cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  نتیجہ = cf_تیاری_جوڑو_ip_happy(cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(!cf->اگلا || !cf->اگلا->connected)
    جاؤ connect_sub_chain;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  نتیجہ = cf_تیاری_جوڑو_socks(cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(!cf->اگلا || !cf->اگلا->connected)
    جاؤ connect_sub_chain;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  نتیجہ = cf_تیاری_جوڑو_http_proxy(cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(!cf->اگلا || !cf->اگلا->connected)
    جاؤ connect_sub_chain;
#اگر_ختم /* !CURL_DISABLE_HTTP */

  نتیجہ = cf_تیاری_جوڑو_haproxy(cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(!cf->اگلا || !cf->اگلا->connected)
    جاؤ connect_sub_chain;
#اگر_ختم /* !CURL_DISABLE_PROXY */

  نتیجہ = cf_تیاری_جوڑو_origin_filters(cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;
  اگر(!cf->اگلا || !cf->اگلا->connected)
    جاؤ connect_sub_chain;

  ctx->state = CF_تیاری_مکمل;
  cf->connected = TRUE;
  *مکمل = TRUE;
  واپس CURLE_OK;
}

ساکن CURLcode cf_تیاری_connect(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 bool *مکمل)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  نتیجہ = cf_تیاری_connect_اقدام(cf, ڈیٹا, مکمل);

  اگر(نتیجہ == CURLE_AGAIN) {
    ++ctx->retry_گنتی;
    اگر(ctx->retry_گنتی > 5) /* arbitrary limit, better just timeout? */
      واپس CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(ڈیٹا, cf, "retrying connect, %d. time", ctx->retry_گنتی);
    Curl_conn_cf_discard_chain(&cf->اگلا, ڈیٹا);
    ctx->state = CF_تیاری_INIT;
    جاؤ retry;
  }
  واپس نتیجہ;
}

ساکن خالی cf_تیاری_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_تیاری_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  curlx_safefree(ctx);
}

ساخت Curl_cftype Curl_cft_تیاری = {
  "SETUP",
  CF_TYPE_تیاری,
  CURL_LOG_LVL_NONE,
  cf_تیاری_destroy,
  cf_تیاری_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساکن CURLcode cf_تیاری_create(ساخت Curl_cfilter **pcf,
                                ساخت Curl_easy *ڈیٹا,
                                uint8_t transport,
                                صحیح ssl_منوال)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_تیاری_ctx *ctx;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)ڈیٹا;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  ctx->state = CF_تیاری_INIT;
  ctx->ssl_منوال = ssl_منوال;
  ctx->transport = transport;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_تیاری, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  ctx = تہی;

برآمد:
  *pcf = نتیجہ ? تہی : cf;
  اگر(ctx) {
    curlx_free(ctx);
  }
  واپس نتیجہ;
}

CURLcode Curl_cf_تیاری_جوڑو(ساخت Curl_easy *ڈیٹا,
                           ساخت connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحیح ssl_منوال)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ڈیٹا);
  نتیجہ = cf_تیاری_create(&cf, ڈیٹا, transport, ssl_منوال);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_جوڑو(ڈیٹا, conn, sockindex, cf);
برآمد:
  واپس نتیجہ;
}

CURLcode Curl_cf_تیاری_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                    ساخت Curl_easy *ڈیٹا,
                                    uint8_t transport,
                                    صحیح ssl_منوال)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  نتیجہ = cf_تیاری_create(&cf, ڈیٹا, transport, ssl_منوال);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
برآمد:
  واپس نتیجہ;
}
