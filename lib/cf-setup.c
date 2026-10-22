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


تعریفنوع شمارش {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

ساختار cf_setup_ctx {
  cf_setup_state state;
  صحیح ssl_mode;
  uint8_t transport;
  uint8_t retry_شمار;
};

#ifndef CURL_DISABLE_PROXY

ایستا CURLcode cf_setup_add_haproxy(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    اگر(data->set.haproxyprotocol) {
      اگر(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        بازگشت CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_درج_after(cf, data);
      اگر(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (صحیح)result);
        بازگشت result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  بازگشت result;
}

ایستا CURLcode cf_setup_add_socks(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  اگر(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    ساختار Curl_peer *second_peer;

    اگر(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    وگرنه
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    اگر(!second_peer)
      بازگشت CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_درج_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    اگر(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (صحیح)result);
      بازگشت result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  بازگشت result;
}

#ifndef CURL_DISABLE_HTTP
ایستا CURLcode cf_setup_add_http_proxy(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    ساختار Curl_peer *peer = cf->conn->http_proxy.peer;
    ساختار Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    اگر(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_درج_after(
        cf, data, cf->conn->http_proxy.peer);
      اگر(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (صحیح)result);
        بازگشت result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_درج_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    اگر(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (صحیح)result);
      بازگشت result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  بازگشت result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ایستا ساختار Curl_peer *conn_get_first_origin(ساختار connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  اگر(conn->socks_proxy.peer)
    بازگشت conn->socks_proxy.peer;
  اگر(conn->http_proxy.peer)
    بازگشت conn->http_proxy.peer;
#endif
  بازگشت (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ایستا CURLcode cf_setup_add_ip_happy(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    ساختار Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    ساختار Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    ساختار Curl_peer *tunnel_peer = پوچ;
    uint8_t first_transport = ctx->transport;

    اگر(!first_peer)
      بازگشت CURLE_FAILED_INIT;

#اگر !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    اگر(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      اگر((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        بازگشت CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_درج_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    اگر(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (صحیح)result);
      بازگشت result;
    }

    اگر(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    وگرنه {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  بازگشت result;
}

ایستا CURLcode cf_setup_add_origin_filters(ساختار Curl_cfilter *cf,
                                            ساختار Curl_easy *data)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (تهی)data; /* not used in all builds */
  اگر(ctx->state < CF_SETUP_CNNCT_SSL) {
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    اگر(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      ساختار Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      ساختار Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_درج_after(cf, data);
      اگر(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (صحیح)result);
        بازگشت result;
      }
      result = Curl_cf_quic_درج_after(cf, data, origin, peer);
      اگر(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (صحیح)result);
        بازگشت result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    وگرنه
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    اگر((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      اگر(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_درج_after(cf, data, cf->conn->origin);
      }
      وگرنه
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_درج_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      اگر(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (صحیح)result);
        بازگشت result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  بازگشت result;
}

ایستا CURLcode cf_setup_connect_steps(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       bool *done)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  اگر(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  اگر(result)
    بازگشت result;
  اگر(!cf->next || !cf->next->connected)
    برو connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  اگر(result)
    بازگشت result;
  اگر(!cf->next || !cf->next->connected)
    برو connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  اگر(result)
    بازگشت result;
  اگر(!cf->next || !cf->next->connected)
    برو connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  اگر(result)
    بازگشت result;
  اگر(!cf->next || !cf->next->connected)
    برو connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  اگر(result)
    بازگشت result;
  اگر(!cf->next || !cf->next->connected)
    برو connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_setup_connect(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 bool *done)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  اگر(result == CURLE_AGAIN) {
    ++ctx->retry_شمار;
    اگر(ctx->retry_شمار > 5) /* arbitrary limit, better just timeout? */
      بازگشت CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_شمار);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    برو retry;
  }
  بازگشت result;
}

ایستا تهی cf_setup_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

ساختار Curl_cftype Curl_cft_setup = {
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

ایستا CURLcode cf_setup_create(ساختار Curl_cfilter **pcf,
                                ساختار Curl_easy *data,
                                uint8_t transport,
                                صحیح ssl_mode)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (تهی)data;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  اگر(result)
    برو out;
  ctx = پوچ;

out:
  *pcf = result ? پوچ : cf;
  اگر(ctx) {
    curlx_free(ctx);
  }
  بازگشت result;
}

CURLcode Curl_cf_setup_add(ساختار Curl_easy *data,
                           ساختار connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحیح ssl_mode)
{
  ساختار Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  اگر(result)
    برو out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  بازگشت result;
}

CURLcode Curl_cf_setup_درج_after(ساختار Curl_cfilter *cf_at,
                                    ساختار Curl_easy *data,
                                    uint8_t transport,
                                    صحیح ssl_mode)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  اگر(result)
    برو out;
  Curl_conn_cf_درج_after(cf_at, cf);
out:
  بازگشت result;
}
