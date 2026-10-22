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


ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

ಸಂರಚನಾ cf_setup_ctx {
  cf_setup_state state;
  ಪೂರ್ಣಾಙ್ಕ ssl_mode;
  uint8_t transport;
  uint8_t retry_ಎಣಿಕೆ;
};

#ifndef CURL_DISABLE_PROXY

ಸ್ಥಿರ CURLcode cf_setup_add_haproxy(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    ಯದಿ(data->set.haproxyprotocol) {
      ಯದಿ(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_ಸೇರಿಸು_after(cf, data);
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_setup_add_socks(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ಯದಿ(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    ಸಂರಚನಾ Curl_peer *second_peer;

    ಯದಿ(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    ಅನ್ಯಥಾ
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    ಯದಿ(!second_peer)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_ಸೇರಿಸು_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
      ಪ್ರತ್ಯಾಗಮ result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_HTTP
ಸ್ಥಿರ CURLcode cf_setup_add_http_proxy(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    ಸಂರಚನಾ Curl_peer *peer = cf->conn->http_proxy.peer;
    ಸಂರಚನಾ Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    ಯದಿ(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_ಸೇರಿಸು_after(
        cf, data, cf->conn->http_proxy.peer);
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_ಸೇರಿಸು_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (ಪೂರ್ಣಾಙ್ಕ)result);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ಸ್ಥಿರ ಸಂರಚನಾ Curl_peer *conn_get_first_origin(ಸಂರಚನಾ connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  ಯದಿ(conn->socks_proxy.peer)
    ಪ್ರತ್ಯಾಗಮ conn->socks_proxy.peer;
  ಯದಿ(conn->http_proxy.peer)
    ಪ್ರತ್ಯಾಗಮ conn->http_proxy.peer;
#endif
  ಪ್ರತ್ಯಾಗಮ (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ಸ್ಥಿರ CURLcode cf_setup_add_ip_happy(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    ಸಂರಚನಾ Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    ಸಂರಚನಾ Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    ಸಂರಚನಾ Curl_peer *tunnel_peer = ರಿಕ್ತ;
    uint8_t first_transport = ctx->transport;

    ಯದಿ(!first_peer)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

#ಯದಿ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    ಯದಿ(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      ಯದಿ((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_ಸೇರಿಸು_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
      ಪ್ರತ್ಯಾಗಮ result;
    }

    ಯದಿ(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_setup_add_origin_filters(ಸಂರಚನಾ Curl_cfilter *cf,
                                            ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)data; /* not used in all builds */
  ಯದಿ(ctx->state < CF_SETUP_CNNCT_SSL) {
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    ಯದಿ(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      ಸಂರಚನಾ Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      ಸಂರಚನಾ Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_ಸೇರಿಸು_after(cf, data);
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      result = Curl_cf_quic_ಸೇರಿಸು_after(cf, data, origin, peer);
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    ಅನ್ಯಥಾ
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    ಯದಿ((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      ಯದಿ(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_ಸೇರಿಸು_after(cf, data, cf->conn->origin);
      }
      ಅನ್ಯಥಾ
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_ಸೇರಿಸು_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_setup_connect_steps(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       bool *done)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  ಯದಿ(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(!cf->next || !cf->next->connected)
    ಗಚ್ಛ connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(!cf->next || !cf->next->connected)
    ಗಚ್ಛ connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(!cf->next || !cf->next->connected)
    ಗಚ್ಛ connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(!cf->next || !cf->next->connected)
    ಗಚ್ಛ connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(!cf->next || !cf->next->connected)
    ಗಚ್ಛ connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_setup_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 bool *done)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  ಯದಿ(result == CURLE_AGAIN) {
    ++ctx->retry_ಎಣಿಕೆ;
    ಯದಿ(ctx->retry_ಎಣಿಕೆ > 5) /* arbitrary limit, better just timeout? */
      ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_ಎಣಿಕೆ);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    ಗಚ್ಛ retry;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_setup_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

ಸಂರಚನಾ Curl_cftype Curl_cft_setup = {
  "SETUP",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_setup_destroy,
  cf_setup_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ಸ್ಥಿರ CURLcode cf_setup_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                ಸಂರಚನಾ Curl_easy *data,
                                uint8_t transport,
                                ಪೂರ್ಣಾಙ್ಕ ssl_mode)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)data;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ctx = ರಿಕ್ತ;

out:
  *pcf = result ? ರಿಕ್ತ : cf;
  ಯದಿ(ctx) {
    curlx_free(ctx);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_setup_add(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ಪೂರ್ಣಾಙ್ಕ ssl_mode)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_setup_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    uint8_t transport,
                                    ಪೂರ್ಣಾಙ್ಕ ssl_mode)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
out:
  ಪ್ರತ್ಯಾಗಮ result;
}
