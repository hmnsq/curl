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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "cf-haproxy.h"
#অন্তর্ভাৱ "cf-ip-happy.h"
#অন্তর্ভাৱ "cf-setup.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "http_proxy.h"
#অন্তর্ভাৱ "socks.h"
#অন্তর্ভাৱ "vquic/cf-capsule.h"
#অন্তর্ভাৱ "vquic/vquic.h"
#অন্তর্ভাৱ "vtls/vtls.h"


প্রকারনির্দেশ গণনা {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

সংরচনা cf_setup_ctx {
  cf_setup_state state;
  পূর্ণাঙ্ক ssl_mode;
  uint8_t transport;
  uint8_t retry_গুনতি;
};

#ifndef CURL_DISABLE_PROXY

স্থির CURLcode cf_setup_add_haproxy(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    যদি(data->set.haproxyprotocol) {
      যদি(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        ঘূৰাই CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_ঢোকাও_after(cf, data);
      যদি(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (পূর্ণাঙ্ক)result);
        ঘূৰাই result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_setup_add_socks(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  যদি(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    সংরচনা Curl_peer *second_peer;

    যদি(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    অন্যথা
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    যদি(!second_peer)
      ঘূৰাই CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_ঢোকাও_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    যদি(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (পূর্ণাঙ্ক)result);
      ঘূৰাই result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  ঘূৰাই result;
}

#ifndef CURL_DISABLE_HTTP
স্থির CURLcode cf_setup_add_http_proxy(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    সংরচনা Curl_peer *peer = cf->conn->http_proxy.peer;
    সংরচনা Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    যদি(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_ঢোকাও_after(
        cf, data, cf->conn->http_proxy.peer);
      যদি(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (পূর্ণাঙ্ক)result);
        ঘূৰাই result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_ঢোকাও_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    যদি(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (পূর্ণাঙ্ক)result);
      ঘূৰাই result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  ঘূৰাই result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
স্থির সংরচনা Curl_peer *conn_get_first_origin(সংরচনা connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  যদি(conn->socks_proxy.peer)
    ঘূৰাই conn->socks_proxy.peer;
  যদি(conn->http_proxy.peer)
    ঘূৰাই conn->http_proxy.peer;
#endif
  ঘূৰাই (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

স্থির CURLcode cf_setup_add_ip_happy(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    সংরচনা Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    সংরচনা Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    সংরচনা Curl_peer *tunnel_peer = রিক্ত;
    uint8_t first_transport = ctx->transport;

    যদি(!first_peer)
      ঘূৰাই CURLE_FAILED_INIT;

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    যদি(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      যদি((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        ঘূৰাই CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_ঢোকাও_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    যদি(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (পূর্ণাঙ্ক)result);
      ঘূৰাই result;
    }

    যদি(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    অন্যথা {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_setup_add_origin_filters(সংরচনা Curl_cfilter *cf,
                                            সংরচনা Curl_easy *data)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (শূন্য)data; /* not used in all builds */
  যদি(ctx->state < CF_SETUP_CNNCT_SSL) {
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    যদি(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      সংরচনা Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      সংরচনা Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_ঢোকাও_after(cf, data);
      যদি(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (পূর্ণাঙ্ক)result);
        ঘূৰাই result;
      }
      result = Curl_cf_quic_ঢোকাও_after(cf, data, origin, peer);
      যদি(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (পূর্ণাঙ্ক)result);
        ঘূৰাই result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    অন্যথা
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    যদি((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      যদি(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_ঢোকাও_after(cf, data, cf->conn->origin);
      }
      অন্যথা
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_ঢোকাও_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      যদি(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (পূর্ণাঙ্ক)result);
        ঘূৰাই result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_setup_connect_steps(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       bool *done)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(cf->connected) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  যদি(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    যদি(result || !*done)
      ঘূৰাই result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  যদি(result)
    ঘূৰাই result;
  যদি(!cf->next || !cf->next->connected)
    গচ্ছ connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  যদি(result)
    ঘূৰাই result;
  যদি(!cf->next || !cf->next->connected)
    গচ্ছ connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  যদি(result)
    ঘূৰাই result;
  যদি(!cf->next || !cf->next->connected)
    গচ্ছ connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  যদি(result)
    ঘূৰাই result;
  যদি(!cf->next || !cf->next->connected)
    গচ্ছ connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  যদি(result)
    ঘূৰাই result;
  যদি(!cf->next || !cf->next->connected)
    গচ্ছ connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode cf_setup_connect(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 bool *done)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  যদি(result == CURLE_AGAIN) {
    ++ctx->retry_গুনতি;
    যদি(ctx->retry_গুনতি > 5) /* arbitrary limit, better just timeout? */
      ঘূৰাই CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_গুনতি);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    গচ্ছ retry;
  }
  ঘূৰাই result;
}

স্থির শূন্য cf_setup_destroy(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

সংরচনা Curl_cftype Curl_cft_setup = {
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

স্থির CURLcode cf_setup_create(সংরচনা Curl_cfilter **pcf,
                                সংরচনা Curl_easy *data,
                                uint8_t transport,
                                পূর্ণাঙ্ক ssl_mode)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (শূন্য)data;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  যদি(result)
    গচ্ছ out;
  ctx = রিক্ত;

out:
  *pcf = result ? রিক্ত : cf;
  যদি(ctx) {
    curlx_free(ctx);
  }
  ঘূৰাই result;
}

CURLcode Curl_cf_setup_add(সংরচনা Curl_easy *data,
                           সংরচনা connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           পূর্ণাঙ্ক ssl_mode)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  যদি(result)
    গচ্ছ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ঘূৰাই result;
}

CURLcode Curl_cf_setup_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                    সংরচনা Curl_easy *data,
                                    uint8_t transport,
                                    পূর্ণাঙ্ক ssl_mode)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  যদি(result)
    গচ্ছ out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
out:
  ঘূৰাই result;
}
