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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "cf-haproxy.h"
#ضمن "cf-ip-happy.h"
#ضمن "cf-setup.h"
#ضمن "curl_trc.h"
#ضمن "connect.h"
#ضمن "http_proxy.h"
#ضمن "socks.h"
#ضمن "vquic/cf-capsule.h"
#ضمن "vquic/vquic.h"
#ضمن "vtls/vtls.h"


تعريفنوع تعداد {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

بنية cf_setup_ctx {
  cf_setup_state state;
  صحيح ssl_mode;
  uint8_t transport;
  uint8_t retry_عدد;
};

#ifndef CURL_DISABLE_PROXY

ساكن CURLcode cf_setup_add_haproxy(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data)
{
  بنية cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  إذا(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    إذا(data->set.haproxyprotocol) {
      إذا(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_أدرج_after(cf, data);
      إذا(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (صحيح)result);
        أرجع result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  أرجع result;
}

ساكن CURLcode cf_setup_add_socks(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  إذا(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    بنية Curl_peer *second_peer;

    إذا(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    وإلا
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    إذا(!second_peer)
      أرجع CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_أدرج_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    إذا(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (صحيح)result);
      أرجع result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  أرجع result;
}

#ifndef CURL_DISABLE_HTTP
ساكن CURLcode cf_setup_add_http_proxy(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data)
{
  بنية cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  إذا(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    بنية Curl_peer *peer = cf->conn->http_proxy.peer;
    بنية Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    إذا(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_أدرج_after(
        cf, data, cf->conn->http_proxy.peer);
      إذا(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (صحيح)result);
        أرجع result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_أدرج_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    إذا(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (صحيح)result);
      أرجع result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  أرجع result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ساكن بنية Curl_peer *conn_get_first_origin(بنية connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#endif
  أرجع (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ساكن CURLcode cf_setup_add_ip_happy(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data)
{
  بنية cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  إذا(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    بنية Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    بنية Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    بنية Curl_peer *tunnel_peer = عدم;
    uint8_t first_transport = ctx->transport;

    إذا(!first_peer)
      أرجع CURLE_FAILED_INIT;

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    إذا(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      إذا((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_أدرج_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    إذا(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (صحيح)result);
      أرجع result;
    }

    إذا(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    وإلا {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  أرجع result;
}

ساكن CURLcode cf_setup_add_origin_filters(بنية Curl_cfilter *cf,
                                            بنية Curl_easy *data)
{
  بنية cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (فراغ)data; /* not used in all builds */
  إذا(ctx->state < CF_SETUP_CNNCT_SSL) {
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    إذا(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      بنية Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      بنية Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_أدرج_after(cf, data);
      إذا(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (صحيح)result);
        أرجع result;
      }
      result = Curl_cf_quic_أدرج_after(cf, data, origin, peer);
      إذا(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (صحيح)result);
        أرجع result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    وإلا
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    إذا((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      إذا(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_أدرج_after(cf, data, cf->conn->origin);
      }
      وإلا
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_أدرج_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      إذا(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (صحيح)result);
        أرجع result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  أرجع result;
}

ساكن CURLcode cf_setup_connect_steps(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       bool *done)
{
  بنية cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  إذا(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    إذا(result || !*done)
      أرجع result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  إذا(result)
    أرجع result;
  إذا(!cf->next || !cf->next->connected)
    اذهب connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  إذا(result)
    أرجع result;
  إذا(!cf->next || !cf->next->connected)
    اذهب connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  إذا(result)
    أرجع result;
  إذا(!cf->next || !cf->next->connected)
    اذهب connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  إذا(result)
    أرجع result;
  إذا(!cf->next || !cf->next->connected)
    اذهب connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  إذا(result)
    أرجع result;
  إذا(!cf->next || !cf->next->connected)
    اذهب connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_setup_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 bool *done)
{
  بنية cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  إذا(result == CURLE_AGAIN) {
    ++ctx->retry_عدد;
    إذا(ctx->retry_عدد > 5) /* arbitrary limit, better just timeout? */
      أرجع CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_عدد);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    اذهب retry;
  }
  أرجع result;
}

ساكن فراغ cf_setup_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

بنية Curl_cftype Curl_cft_setup = {
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

ساكن CURLcode cf_setup_create(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *data,
                                uint8_t transport,
                                صحيح ssl_mode)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (فراغ)data;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  إذا(result)
    اذهب out;
  ctx = عدم;

out:
  *pcf = result ? عدم : cf;
  إذا(ctx) {
    curlx_free(ctx);
  }
  أرجع result;
}

CURLcode Curl_cf_setup_add(بنية Curl_easy *data,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحيح ssl_mode)
{
  بنية Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  إذا(result)
    اذهب out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  أرجع result;
}

CURLcode Curl_cf_setup_أدرج_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *data,
                                    uint8_t transport,
                                    صحيح ssl_mode)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  إذا(result)
    اذهب out;
  Curl_conn_cf_أدرج_after(cf_at, cf);
out:
  أرجع result;
}
