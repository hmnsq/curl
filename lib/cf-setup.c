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

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-haproxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-ip-happy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socks.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-capsule.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CF_ਤਿਆਰੀ_INIT,
  CF_ਤਿਆਰੀ_CNNCT_EYEBALLS,
  CF_ਤਿਆਰੀ_CNNCT_SOCKS,
  CF_ਤਿਆਰੀ_CNNCT_HTTP_PROXY,
  CF_ਤਿਆਰੀ_CNNCT_HAPROXY,
  CF_ਤਿਆਰੀ_CNNCT_SSL,
  CF_ਤਿਆਰੀ_ਪੂਰਾ
} cf_ਤਿਆਰੀ_state;

ਬਣਤਰ cf_ਤਿਆਰੀ_ctx {
  cf_ਤਿਆਰੀ_state state;
  ਪੂਰਨ ssl_ਬਹੁਲਕ;
  uint8_t transport;
  uint8_t retry_ਗਿਣਤੀ;
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_ਜੋੜੋ_haproxy(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ctx->state < CF_ਤਿਆਰੀ_CNNCT_HAPROXY) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.haproxyprotocol) {
      ਜੇ(ctx->transport == TRNSPRT_QUIC) {
        failf(ਅੰਕੜੇ, "haproxy protocol does not support QUIC");
        ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
      }
      ਨਤੀਜਾ = Curl_cf_haproxy_ਪਾਓ_after(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding HAPROXY filter failed -> %d",
                    (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "added HAPROXY filter");
    }
    ctx->state = CF_ਤਿਆਰੀ_CNNCT_HAPROXY;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_ਜੋੜੋ_socks(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(ctx->state < CF_ਤਿਆਰੀ_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    ਬਣਤਰ Curl_peer *ਦੂਜਾ_peer;

    ਜੇ(cf->conn->http_proxy.peer)
      ਦੂਜਾ_peer = cf->conn->http_proxy.peer;
    ਨਹੀਂਤਾਂ
      ਦੂਜਾ_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    ਜੇ(!ਦੂਜਾ_peer)
      ਵਾਪਸ CURLE_FAILED_INIT;

    ਨਤੀਜਾ = Curl_cf_socks_proxy_ਪਾਓ_after(
      cf, ਅੰਕੜੇ, ਦੂਜਾ_peer, cf->conn->ip_ਸੰਸਕਰਣ,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding SOCKS filter failed -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "added SOCKS filter to %s:%u",
                ਦੂਜਾ_peer->hostname, ਦੂਜਾ_peer->ਪੋਰਟ);
    ctx->state = CF_ਤਿਆਰੀ_CNNCT_SOCKS;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_ਜੋੜੋ_http_proxy(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ctx->state < CF_ਤਿਆਰੀ_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->ਬਿੱਟ.origin_is_proxy) {
    ਬਣਤਰ Curl_peer *peer = cf->conn->http_proxy.peer;
    ਬਣਤਰ Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      ਨਤੀਜਾ = Curl_cf_ssl_proxy_ਪਾਓ_after(
        cf, ਅੰਕੜੇ, cf->conn->http_proxy.peer);
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "added SSL filter for HTTP proxy");
    }
#ਜੇ_ਅੰਤ /* USE_SSL */

    ਨਤੀਜਾ = Curl_cf_http_proxy_ਪਾਓ_after(
      cf, ਅੰਕੜੇ, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (ਪੂਰਨ)ਨਤੀਜਾ);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_ਤਿਆਰੀ_CNNCT_HTTP_PROXY;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ਸਥਿਰ ਬਣਤਰ Curl_peer *conn_get_ਪਹਿਲਾ_origin(ਬਣਤਰ connectdata *conn,
                                             int8_t sockindex)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(conn->socks_proxy.peer)
    ਵਾਪਸ conn->socks_proxy.peer;
  ਜੇ(conn->http_proxy.peer)
    ਵਾਪਸ conn->http_proxy.peer;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_ਜੋੜੋ_ip_happy(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ctx->state < CF_ਤਿਆਰੀ_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    ਬਣਤਰ Curl_peer *ਪਹਿਲਾ_origin =
      conn_get_ਪਹਿਲਾ_origin(cf->conn, cf->sockindex);
    ਬਣਤਰ Curl_peer *ਪਹਿਲਾ_peer =
      Curl_conn_get_ਪਹਿਲਾ_peer(cf->conn, cf->sockindex);
    ਬਣਤਰ Curl_peer *tunnel_peer = ਨਿਰਾ;
    uint8_t ਪਹਿਲਾ_transport = ctx->transport;

    ਜੇ(!ਪਹਿਲਾ_peer)
      ਵਾਪਸ CURLE_FAILED_INIT;

#ਜੇ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    ਜੇ(cf->conn->http_proxy.peer && !cf->conn->ਬਿੱਟ.origin_is_proxy) {
      ਪਹਿਲਾ_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      ਜੇ((ਪਹਿਲਾ_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(ਅੰਕੜੇ, "HTTP/3 proxy not possible via SOCKS");
        ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    ਨਤੀਜਾ = Curl_cf_ip_happy_ਪਾਓ_after(cf, ਅੰਕੜੇ, ਪਹਿਲਾ_origin, ਪਹਿਲਾ_peer,
                                           ਪਹਿਲਾ_transport,
                                           tunnel_peer, ctx->transport);
    ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding happy eyeballs failed -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਜੇ(tunnel_peer && (ਪਹਿਲਾ_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  ਪਹਿਲਾ_peer->hostname, ਪਹਿਲਾ_peer->ਪੋਰਟ);
      ctx->state = CF_ਤਿਆਰੀ_CNNCT_HTTP_PROXY;
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  ਪਹਿਲਾ_peer->hostname, ਪਹਿਲਾ_peer->ਪੋਰਟ);
      ctx->state = CF_ਤਿਆਰੀ_CNNCT_EYEBALLS;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_ਜੋੜੋ_origin_filters(ਬਣਤਰ Curl_cfilter *cf,
                                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)ਅੰਕੜੇ; /* not used in all builds */
  ਜੇ(ctx->state < CF_ਤਿਆਰੀ_CNNCT_SSL) {
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    ਜੇ(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->ਬਿੱਟ.origin_is_proxy) {
      ਬਣਤਰ Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      ਬਣਤਰ Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      ਨਤੀਜਾ = Curl_cf_capsule_ਪਾਓ_after(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding capsule filter failed -> %d",
                    (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਤੀਜਾ = Curl_cf_quic_ਪਾਓ_after(cf, ਅੰਕੜੇ, origin, peer);
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding QUIC filter failed -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "added QUIC filter for origin");
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    ਜੇ((ctx->ssl_ਬਹੁਲਕ == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_ਬਹੁਲਕ != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
      ਜੇ(cf->conn->ਬਿੱਟ.origin_is_proxy) {
        ਨਤੀਜਾ = Curl_cf_ssl_proxy_ਪਾਓ_after(cf, ਅੰਕੜੇ, cf->conn->origin);
      }
      ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        ਨਤੀਜਾ = Curl_cf_ssl_ਪਾਓ_after(cf, ਅੰਕੜੇ,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "adding SSL filter for origin failed -> %d",
                    (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "added SSL filter for origin");
    }
#ਜੇ_ਅੰਤ /* USE_SSL */
    ctx->state = CF_ਤਿਆਰੀ_CNNCT_SSL;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_connect_ਕਦਮਾਂ(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(ਅੰਕੜੇ, cf->sockindex, "cf_setup_connect"));

  ਜੇ(cf->ਅਗਲਾ && !cf->ਅਗਲਾ->connected) {
    ਨਤੀਜਾ = Curl_conn_cf_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_ਜੋੜੋ_ip_happy(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->connected)
    ਜਾਓ connect_sub_chain;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_ਜੋੜੋ_socks(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->connected)
    ਜਾਓ connect_sub_chain;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_ਜੋੜੋ_http_proxy(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->connected)
    ਜਾਓ connect_sub_chain;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */

  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_ਜੋੜੋ_haproxy(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->connected)
    ਜਾਓ connect_sub_chain;
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */

  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_ਜੋੜੋ_origin_filters(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->connected)
    ਜਾਓ connect_sub_chain;

  ctx->state = CF_ਤਿਆਰੀ_ਪੂਰਾ;
  cf->connected = TRUE;
  *ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_connect(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_connect_ਕਦਮਾਂ(cf, ਅੰਕੜੇ, ਪੂਰਾ);

  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    ++ctx->retry_ਗਿਣਤੀ;
    ਜੇ(ctx->retry_ਗਿਣਤੀ > 5) /* arbitrary limit, better just timeout? */
      ਵਾਪਸ CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "retrying connect, %d. time", ctx->retry_ਗਿਣਤੀ);
    Curl_conn_cf_discard_chain(&cf->ਅਗਲਾ, ਅੰਕੜੇ);
    ctx->state = CF_ਤਿਆਰੀ_INIT;
    ਜਾਓ retry;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_ਤਿਆਰੀ_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  curlx_safefree(ctx);
}

ਬਣਤਰ Curl_cftype Curl_cft_ਤਿਆਰੀ = {
  "SETUP",
  CF_TYPE_ਤਿਆਰੀ,
  CURL_LOG_LVL_NONE,
  cf_ਤਿਆਰੀ_destroy,
  cf_ਤਿਆਰੀ_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ਸਥਿਰ CURLcode cf_ਤਿਆਰੀ_create(ਬਣਤਰ Curl_cfilter **pcf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                uint8_t transport,
                                ਪੂਰਨ ssl_ਬਹੁਲਕ)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_ਤਿਆਰੀ_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ctx->state = CF_ਤਿਆਰੀ_INIT;
  ctx->ssl_ਬਹੁਲਕ = ssl_ਬਹੁਲਕ;
  ctx->transport = transport;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_ਤਿਆਰੀ, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = ਨਿਰਾ;

ਬਾਹਰਲਾ:
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  ਜੇ(ctx) {
    curlx_free(ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ਤਿਆਰੀ_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ਪੂਰਨ ssl_ਬਹੁਲਕ)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_create(&cf, ਅੰਕੜੇ, transport, ssl_ਬਹੁਲਕ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, cf);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ਤਿਆਰੀ_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    uint8_t transport,
                                    ਪੂਰਨ ssl_ਬਹੁਲਕ)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਨਤੀਜਾ = cf_ਤਿਆਰੀ_create(&cf, ਅੰਕੜੇ, transport, ssl_ਬਹੁਲਕ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}
