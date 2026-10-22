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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-haproxy.h"
#ಅನ್ತರ್ಭಾವ "cf-ip-happy.h"
#ಅನ್ತರ್ಭಾವ "cf-setup.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "socks.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-capsule.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CF_ಸಿದ್ಧತೆ_INIT,
  CF_ಸಿದ್ಧತೆ_CNNCT_EYEBALLS,
  CF_ಸಿದ್ಧತೆ_CNNCT_SOCKS,
  CF_ಸಿದ್ಧತೆ_CNNCT_HTTP_PROXY,
  CF_ಸಿದ್ಧತೆ_CNNCT_HAPROXY,
  CF_ಸಿದ್ಧತೆ_CNNCT_SSL,
  CF_ಸಿದ್ಧತೆ_ಮುಗಿದ
} cf_ಸಿದ್ಧತೆ_state;

ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx {
  cf_ಸಿದ್ಧತೆ_state state;
  ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ;
  uint8_t transport;
  uint8_t retry_ಎಣಿಕೆ;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_haproxy(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ctx->state < CF_ಸಿದ್ಧತೆ_CNNCT_HAPROXY) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.haproxyprotocol) {
      ಆದರೆ(ctx->transport == TRNSPRT_QUIC) {
        failf(ದತ್ತಾಂಶ, "haproxy protocol does not support QUIC");
        ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
      }
      ಫಲಿತಾಂಶ = Curl_cf_haproxy_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding HAPROXY filter failed -> %d",
                    (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added HAPROXY filter");
    }
    ctx->state = CF_ಸಿದ್ಧತೆ_CNNCT_HAPROXY;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_socks(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(ctx->state < CF_ಸಿದ್ಧತೆ_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    ರಚನೆ Curl_peer *ಎರಡನೇ_peer;

    ಆದರೆ(cf->conn->http_proxy.peer)
      ಎರಡನೇ_peer = cf->conn->http_proxy.peer;
    ಇಲ್ಲದಿದ್ದರೆ
      ಎರಡನೇ_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    ಆದರೆ(!ಎರಡನೇ_peer)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

    ಫಲಿತಾಂಶ = Curl_cf_socks_proxy_ಸೇರಿಸು_after(
      cf, ದತ್ತಾಂಶ, ಎರಡನೇ_peer, cf->conn->ip_ಆವೃತ್ತಿ,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding SOCKS filter failed -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added SOCKS filter to %s:%u",
                ಎರಡನೇ_peer->hostname, ಎರಡನೇ_peer->ಪೋರ್ಟ್);
    ctx->state = CF_ಸಿದ್ಧತೆ_CNNCT_SOCKS;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_http_proxy(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ctx->state < CF_ಸಿದ್ಧತೆ_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
    ರಚನೆ Curl_peer *peer = cf->conn->http_proxy.peer;
    ರಚನೆ Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      ಫಲಿತಾಂಶ = Curl_cf_ssl_proxy_ಸೇರಿಸು_after(
        cf, ದತ್ತಾಂಶ, cf->conn->http_proxy.peer);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added SSL filter for HTTP proxy");
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */

    ಫಲಿತಾಂಶ = Curl_cf_http_proxy_ಸೇರಿಸು_after(
      cf, ದತ್ತಾಂಶ, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_ಸಿದ್ಧತೆ_CNNCT_HTTP_PROXY;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ಸ್ಥಿರ ರಚನೆ Curl_peer *conn_get_ಮೊದಲ_origin(ರಚನೆ connectdata *conn,
                                             int8_t sockindex)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(conn->socks_proxy.peer)
    ಹಿಂತಿರುಗಿ conn->socks_proxy.peer;
  ಆದರೆ(conn->http_proxy.peer)
    ಹಿಂತಿರುಗಿ conn->http_proxy.peer;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_ip_happy(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ctx->state < CF_ಸಿದ್ಧತೆ_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    ರಚನೆ Curl_peer *ಮೊದಲ_origin =
      conn_get_ಮೊದಲ_origin(cf->conn, cf->sockindex);
    ರಚನೆ Curl_peer *ಮೊದಲ_peer =
      Curl_conn_get_ಮೊದಲ_peer(cf->conn, cf->sockindex);
    ರಚನೆ Curl_peer *tunnel_peer = ಖಾಲಿ;
    uint8_t ಮೊದಲ_transport = ctx->transport;

    ಆದರೆ(!ಮೊದಲ_peer)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    ಆದರೆ(cf->conn->http_proxy.peer && !cf->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
      ಮೊದಲ_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      ಆದರೆ((ಮೊದಲ_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(ದತ್ತಾಂಶ, "HTTP/3 proxy not possible via SOCKS");
        ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    ಫಲಿತಾಂಶ = Curl_cf_ip_happy_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, ಮೊದಲ_origin, ಮೊದಲ_peer,
                                           ಮೊದಲ_transport,
                                           tunnel_peer, ctx->transport);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding happy eyeballs failed -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಆದರೆ(tunnel_peer && (ಮೊದಲ_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  ಮೊದಲ_peer->hostname, ಮೊದಲ_peer->ಪೋರ್ಟ್);
      ctx->state = CF_ಸಿದ್ಧತೆ_CNNCT_HTTP_PROXY;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  ಮೊದಲ_peer->hostname, ಮೊದಲ_peer->ಪೋರ್ಟ್);
      ctx->state = CF_ಸಿದ್ಧತೆ_CNNCT_EYEBALLS;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_origin_filters(ರಚನೆ Curl_cfilter *cf,
                                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ; /* not used in all builds */
  ಆದರೆ(ctx->state < CF_ಸಿದ್ಧತೆ_CNNCT_SSL) {
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    ಆದರೆ(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
      ರಚನೆ Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      ರಚನೆ Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      ಫಲಿತಾಂಶ = Curl_cf_capsule_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding capsule filter failed -> %d",
                    (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಫಲಿತಾಂಶ = Curl_cf_quic_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, origin, peer);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding QUIC filter failed -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added QUIC filter for origin");
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    ಆದರೆ((ctx->ssl_ಬಹುಲಕ == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_ಬಹುಲಕ != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
      ಆದರೆ(cf->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
        ಫಲಿತಾಂಶ = Curl_cf_ssl_proxy_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, cf->conn->origin);
      }
      ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        ಫಲಿತಾಂಶ = Curl_cf_ssl_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adding SSL filter for origin failed -> %d",
                    (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added SSL filter for origin");
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */
    ctx->state = CF_ಸಿದ್ಧತೆ_CNNCT_SSL;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_connect_ಹೆಜ್ಜೆಗಳು(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool *ಮುಗಿದ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(ದತ್ತಾಂಶ, cf->sockindex, "cf_setup_connect"));

  ಆದರೆ(cf->ಮುಂದಿನ && !cf->ಮುಂದಿನ->connected) {
    ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_ip_happy(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->connected)
    ಹೋಗು connect_sub_chain;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_socks(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->connected)
    ಹೋಗು connect_sub_chain;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_http_proxy(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->connected)
    ಹೋಗು connect_sub_chain;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */

  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_haproxy(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->connected)
    ಹೋಗು connect_sub_chain;
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */

  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು_origin_filters(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->connected)
    ಹೋಗು connect_sub_chain;

  ctx->state = CF_ಸಿದ್ಧತೆ_ಮುಗಿದ;
  cf->connected = TRUE;
  *ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_connect(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool *ಮುಗಿದ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_connect_ಹೆಜ್ಜೆಗಳು(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    ++ctx->retry_ಎಣಿಕೆ;
    ಆದರೆ(ctx->retry_ಎಣಿಕೆ > 5) /* arbitrary limit, better just timeout? */
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "retrying connect, %d. time", ctx->retry_ಎಣಿಕೆ);
    Curl_conn_cf_discard_chain(&cf->ಮುಂದಿನ, ದತ್ತಾಂಶ);
    ctx->state = CF_ಸಿದ್ಧತೆ_INIT;
    ಹೋಗು retry;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ಸಿದ್ಧತೆ_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  curlx_safefree(ctx);
}

ರಚನೆ Curl_cftype Curl_cft_ಸಿದ್ಧತೆ = {
  "SETUP",
  CF_TYPE_ಸಿದ್ಧತೆ,
  CURL_LOG_LVL_NONE,
  cf_ಸಿದ್ಧತೆ_destroy,
  cf_ಸಿದ್ಧತೆ_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ಸ್ಥಿರ CURLcode cf_ಸಿದ್ಧತೆ_create(ರಚನೆ Curl_cfilter **pcf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                uint8_t transport,
                                ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_ಸಿದ್ಧತೆ_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ctx->state = CF_ಸಿದ್ಧತೆ_INIT;
  ctx->ssl_ಬಹುಲಕ = ssl_ಬಹುಲಕ;
  ctx->transport = transport;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_ಸಿದ್ಧತೆ, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = ಖಾಲಿ;

ಹೊರಹೋಗು:
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  ಆದರೆ(ctx) {
    curlx_free(ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ಸಿದ್ಧತೆ_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_create(&cf, ದತ್ತಾಂಶ, transport, ssl_ಬಹುಲಕ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, cf);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ಸಿದ್ಧತೆ_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    uint8_t transport,
                                    ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = cf_ಸಿದ್ಧತೆ_create(&cf, ದತ್ತಾಂಶ, transport, ssl_ಬಹುಲಕ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
