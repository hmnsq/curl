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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-haproxy.h"
#അന്തര്ഭാവ "cf-ip-happy.h"
#അന്തര്ഭാവ "cf-setup.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "socks.h"
#അന്തര്ഭാവ "vquic/cf-capsule.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "vtls/vtls.h"


തരനിർവചനം എണ്ണൽ {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

ഘടന cf_setup_ctx {
  cf_setup_state state;
  പൂർണ്ണസംഖ്യ ssl_mode;
  uint8_t transport;
  uint8_t retry_എണ്ണം;
};

#ifndef CURL_DISABLE_PROXY

സ്ഥിതം CURLcode cf_setup_add_haproxy(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    എങ്കിൽ(data->set.haproxyprotocol) {
      എങ്കിൽ(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_തിരുകുക_after(cf, data);
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_setup_add_socks(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  എങ്കിൽ(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    ഘടന Curl_peer *second_peer;

    എങ്കിൽ(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    അല്ലെങ്കിൽ
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    എങ്കിൽ(!second_peer)
      തിരികെ CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_തിരുകുക_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (പൂർണ്ണസംഖ്യ)result);
      തിരികെ result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  തിരികെ result;
}

#ifndef CURL_DISABLE_HTTP
സ്ഥിതം CURLcode cf_setup_add_http_proxy(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    ഘടന Curl_peer *peer = cf->conn->http_proxy.peer;
    ഘടന Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    എങ്കിൽ(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_തിരുകുക_after(
        cf, data, cf->conn->http_proxy.peer);
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_തിരുകുക_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (പൂർണ്ണസംഖ്യ)result);
      തിരികെ result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  തിരികെ result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
സ്ഥിതം ഘടന Curl_peer *conn_get_first_origin(ഘടന connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(conn->socks_proxy.peer)
    തിരികെ conn->socks_proxy.peer;
  എങ്കിൽ(conn->http_proxy.peer)
    തിരികെ conn->http_proxy.peer;
#endif
  തിരികെ (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

സ്ഥിതം CURLcode cf_setup_add_ip_happy(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    ഘടന Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    ഘടന Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    ഘടന Curl_peer *tunnel_peer = ശൂന്യ;
    uint8_t first_transport = ctx->transport;

    എങ്കിൽ(!first_peer)
      തിരികെ CURLE_FAILED_INIT;

#എങ്കിൽ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    എങ്കിൽ(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      എങ്കിൽ((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_തിരുകുക_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (പൂർണ്ണസംഖ്യ)result);
      തിരികെ result;
    }

    എങ്കിൽ(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_setup_add_origin_filters(ഘടന Curl_cfilter *cf,
                                            ഘടന Curl_easy *data)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)data; /* not used in all builds */
  എങ്കിൽ(ctx->state < CF_SETUP_CNNCT_SSL) {
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    എങ്കിൽ(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      ഘടന Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      ഘടന Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_തിരുകുക_after(cf, data);
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      result = Curl_cf_quic_തിരുകുക_after(cf, data, origin, peer);
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    അല്ലെങ്കിൽ
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    എങ്കിൽ((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      എങ്കിൽ(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_തിരുകുക_after(cf, data, cf->conn->origin);
      }
      അല്ലെങ്കിൽ
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_തിരുകുക_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_setup_connect_steps(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       bool *done)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  എങ്കിൽ(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(!cf->next || !cf->next->connected)
    പോകുക connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(!cf->next || !cf->next->connected)
    പോകുക connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(!cf->next || !cf->next->connected)
    പോകുക connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(!cf->next || !cf->next->connected)
    പോകുക connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(!cf->next || !cf->next->connected)
    പോകുക connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_setup_connect(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 bool *done)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  എങ്കിൽ(result == CURLE_AGAIN) {
    ++ctx->retry_എണ്ണം;
    എങ്കിൽ(ctx->retry_എണ്ണം > 5) /* arbitrary limit, better just timeout? */
      തിരികെ CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_എണ്ണം);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    പോകുക retry;
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_setup_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

ഘടന Curl_cftype Curl_cft_setup = {
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

സ്ഥിതം CURLcode cf_setup_create(ഘടന Curl_cfilter **pcf,
                                ഘടന Curl_easy *data,
                                uint8_t transport,
                                പൂർണ്ണസംഖ്യ ssl_mode)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)data;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  ctx = ശൂന്യ;

out:
  *pcf = result ? ശൂന്യ : cf;
  എങ്കിൽ(ctx) {
    curlx_free(ctx);
  }
  തിരികെ result;
}

CURLcode Curl_cf_setup_add(ഘടന Curl_easy *data,
                           ഘടന connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           പൂർണ്ണസംഖ്യ ssl_mode)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  തിരികെ result;
}

CURLcode Curl_cf_setup_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                    ഘടന Curl_easy *data,
                                    uint8_t transport,
                                    പൂർണ്ണസംഖ്യ ssl_mode)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_തിരുകുക_after(cf_at, cf);
out:
  തിരികെ result;
}
