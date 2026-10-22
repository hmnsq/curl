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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-haproxy.h"
#அந்தர்பாவ "cf-ip-happy.h"
#அந்தர்பாவ "cf-setup.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "socks.h"
#அந்தர்பாவ "vquic/cf-capsule.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "vtls/vtls.h"


வகைவரை எண்ணிக்கை {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

கட்டமைப்பு cf_setup_ctx {
  cf_setup_state state;
  முழு ssl_mode;
  uint8_t transport;
  uint8_t retry_மொத்தம்;
};

#ifndef CURL_DISABLE_PROXY

நிலையான CURLcode cf_setup_add_haproxy(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    எனில்(data->set.haproxyprotocol) {
      எனில்(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_செருகு_after(cf, data);
      எனில்(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (முழு)result);
        திரும்பு result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_setup_add_socks(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  எனில்(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    கட்டமைப்பு Curl_peer *second_peer;

    எனில்(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    இல்லையெனில்
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    எனில்(!second_peer)
      திரும்பு CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_செருகு_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    எனில்(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (முழு)result);
      திரும்பு result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  திரும்பு result;
}

#ifndef CURL_DISABLE_HTTP
நிலையான CURLcode cf_setup_add_http_proxy(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    கட்டமைப்பு Curl_peer *peer = cf->conn->http_proxy.peer;
    கட்டமைப்பு Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    எனில்(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_செருகு_after(
        cf, data, cf->conn->http_proxy.peer);
      எனில்(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (முழு)result);
        திரும்பு result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_செருகு_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    எனில்(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (முழு)result);
      திரும்பு result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  திரும்பு result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
நிலையான கட்டமைப்பு Curl_peer *conn_get_first_origin(கட்டமைப்பு connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  எனில்(conn->socks_proxy.peer)
    திரும்பு conn->socks_proxy.peer;
  எனில்(conn->http_proxy.peer)
    திரும்பு conn->http_proxy.peer;
#endif
  திரும்பு (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

நிலையான CURLcode cf_setup_add_ip_happy(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    கட்டமைப்பு Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    கட்டமைப்பு Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    கட்டமைப்பு Curl_peer *tunnel_peer = வெற்று;
    uint8_t first_transport = ctx->transport;

    எனில்(!first_peer)
      திரும்பு CURLE_FAILED_INIT;

#எனில் !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    எனில்(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      எனில்((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_செருகு_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    எனில்(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (முழு)result);
      திரும்பு result;
    }

    எனில்(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    இல்லையெனில் {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  திரும்பு result;
}

நிலையான CURLcode cf_setup_add_origin_filters(கட்டமைப்பு Curl_cfilter *cf,
                                            கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (வெறுமை)data; /* not used in all builds */
  எனில்(ctx->state < CF_SETUP_CNNCT_SSL) {
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    எனில்(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      கட்டமைப்பு Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      கட்டமைப்பு Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_செருகு_after(cf, data);
      எனில்(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (முழு)result);
        திரும்பு result;
      }
      result = Curl_cf_quic_செருகு_after(cf, data, origin, peer);
      எனில்(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (முழு)result);
        திரும்பு result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    இல்லையெனில்
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    எனில்((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      எனில்(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_செருகு_after(cf, data, cf->conn->origin);
      }
      இல்லையெனில்
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_செருகு_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      எனில்(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (முழு)result);
        திரும்பு result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_setup_connect_steps(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       bool *done)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  எனில்(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  எனில்(result)
    திரும்பு result;
  எனில்(!cf->next || !cf->next->connected)
    செல் connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  எனில்(result)
    திரும்பு result;
  எனில்(!cf->next || !cf->next->connected)
    செல் connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  எனில்(result)
    திரும்பு result;
  எனில்(!cf->next || !cf->next->connected)
    செல் connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  எனில்(result)
    திரும்பு result;
  எனில்(!cf->next || !cf->next->connected)
    செல் connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  எனில்(result)
    திரும்பு result;
  எனில்(!cf->next || !cf->next->connected)
    செல் connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_setup_connect(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 bool *done)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  எனில்(result == CURLE_AGAIN) {
    ++ctx->retry_மொத்தம்;
    எனில்(ctx->retry_மொத்தம் > 5) /* arbitrary limit, better just timeout? */
      திரும்பு CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_மொத்தம்);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    செல் retry;
  }
  திரும்பு result;
}

நிலையான வெறுமை cf_setup_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

கட்டமைப்பு Curl_cftype Curl_cft_setup = {
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

நிலையான CURLcode cf_setup_create(கட்டமைப்பு Curl_cfilter **pcf,
                                கட்டமைப்பு Curl_easy *data,
                                uint8_t transport,
                                முழு ssl_mode)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (வெறுமை)data;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  எனில்(result)
    செல் out;
  ctx = வெற்று;

out:
  *pcf = result ? வெற்று : cf;
  எனில்(ctx) {
    curlx_free(ctx);
  }
  திரும்பு result;
}

CURLcode Curl_cf_setup_add(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           முழு ssl_mode)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  எனில்(result)
    செல் out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  திரும்பு result;
}

CURLcode Curl_cf_setup_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                    கட்டமைப்பு Curl_easy *data,
                                    uint8_t transport,
                                    முழு ssl_mode)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  எனில்(result)
    செல் out;
  Curl_conn_cf_செருகு_after(cf_at, cf);
out:
  திரும்பு result;
}
