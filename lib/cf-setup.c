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
  CF_تیاری_INIT,
  CF_تیاری_CNNCT_EYEBALLS,
  CF_تیاری_CNNCT_SOCKS,
  CF_تیاری_CNNCT_HTTP_PROXY,
  CF_تیاری_CNNCT_HAPROXY,
  CF_تیاری_CNNCT_SSL,
  CF_تیاری_بشپړ
} cf_تیاری_state;

بنية cf_تیاری_ctx {
  cf_تیاری_state state;
  صحيح ssl_ډېرپېښ;
  uint8_t transport;
  uint8_t retry_شمېر;
};

#که_نه_تعریف_شوی CURL_DISABLE_PROXY

ساكن CURLcode cf_تیاری_جمع_کړه_haproxy(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  إذا(ctx->state < CF_تیاری_CNNCT_HAPROXY) {
    إذا(معلومات->سېټ.haproxyprotocol) {
      إذا(ctx->transport == TRNSPRT_QUIC) {
        failf(معلومات, "haproxy protocol does not support QUIC");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
      پایله = Curl_cf_haproxy_ورننباسه_after(cf, معلومات);
      إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "adding HAPROXY filter failed -> %d",
                    (صحيح)پایله);
        أرجع پایله;
      }
      CURL_TRC_CF(معلومات, cf, "added HAPROXY filter");
    }
    ctx->state = CF_تیاری_CNNCT_HAPROXY;
  }
  أرجع پایله;
}

ساكن CURLcode cf_تیاری_جمع_کړه_socks(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  إذا(ctx->state < CF_تیاری_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    بنية Curl_peer *دوهم_peer;

    إذا(cf->conn->http_proxy.peer)
      دوهم_peer = cf->conn->http_proxy.peer;
    وإلا
      دوهم_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    إذا(!دوهم_peer)
      أرجع CURLE_FAILED_INIT;

    پایله = Curl_cf_socks_proxy_ورننباسه_after(
      cf, معلومات, دوهم_peer, cf->conn->ip_نسخه,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    إذا(پایله) {
      CURL_TRC_CF(معلومات, cf, "adding SOCKS filter failed -> %d", (صحيح)پایله);
      أرجع پایله;
    }

    CURL_TRC_CF(معلومات, cf, "added SOCKS filter to %s:%u",
                دوهم_peer->hostname, دوهم_peer->پورټ);
    ctx->state = CF_تیاری_CNNCT_SOCKS;
  }
  أرجع پایله;
}

#که_نه_تعریف_شوی CURL_DISABLE_HTTP
ساكن CURLcode cf_تیاری_جمع_کړه_http_proxy(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  إذا(ctx->state < CF_تیاری_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->بټونه.origin_is_proxy) {
    بنية Curl_peer *peer = cf->conn->http_proxy.peer;
    بنية Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#که_تعریف_شوی USE_SSL
    إذا(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      پایله = Curl_cf_ssl_proxy_ورننباسه_after(
        cf, معلومات, cf->conn->http_proxy.peer);
      إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (صحيح)پایله);
        أرجع پایله;
      }
      CURL_TRC_CF(معلومات, cf, "added SSL filter for HTTP proxy");
    }
#د_که_پای /* USE_SSL */

    پایله = Curl_cf_http_proxy_ورننباسه_after(
      cf, معلومات, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    إذا(پایله) {
      CURL_TRC_CF(معلومات, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (صحيح)پایله);
      أرجع پایله;
    }
    CURL_TRC_CF(معلومات, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_تیاری_CNNCT_HTTP_PROXY;
  }
  أرجع پایله;
}
#د_که_پای /* !CURL_DISABLE_HTTP */
#د_که_پای /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ساكن بنية Curl_peer *conn_get_لومړی_origin(بنية connectdata *conn,
                                             int8_t sockindex)
{
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#د_که_پای
  أرجع (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ساكن CURLcode cf_تیاری_جمع_کړه_ip_happy(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  إذا(ctx->state < CF_تیاری_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    بنية Curl_peer *لومړی_origin =
      conn_get_لومړی_origin(cf->conn, cf->sockindex);
    بنية Curl_peer *لومړی_peer =
      Curl_conn_get_لومړی_peer(cf->conn, cf->sockindex);
    بنية Curl_peer *tunnel_peer = عدم;
    uint8_t لومړی_transport = ctx->transport;

    إذا(!لومړی_peer)
      أرجع CURLE_FAILED_INIT;

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    إذا(cf->conn->http_proxy.peer && !cf->conn->بټونه.origin_is_proxy) {
      لومړی_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      إذا((لومړی_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(معلومات, "HTTP/3 proxy not possible via SOCKS");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#د_که_پای /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    پایله = Curl_cf_ip_happy_ورننباسه_after(cf, معلومات, لومړی_origin, لومړی_peer,
                                           لومړی_transport,
                                           tunnel_peer, ctx->transport);
    إذا(پایله) {
      CURL_TRC_CF(معلومات, cf, "adding happy eyeballs failed -> %d", (صحيح)پایله);
      أرجع پایله;
    }

    إذا(tunnel_peer && (لومړی_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(معلومات, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  لومړی_peer->hostname, لومړی_peer->پورټ);
      ctx->state = CF_تیاری_CNNCT_HTTP_PROXY;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  لومړی_peer->hostname, لومړی_peer->پورټ);
      ctx->state = CF_تیاری_CNNCT_EYEBALLS;
    }
  }
  أرجع پایله;
}

ساكن CURLcode cf_تیاری_جمع_کړه_origin_filters(بنية Curl_cfilter *cf,
                                            بنية Curl_easy *معلومات)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  (فراغ)معلومات; /* not used in all builds */
  إذا(ctx->state < CF_تیاری_CNNCT_SSL) {
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    إذا(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->بټونه.origin_is_proxy) {
      بنية Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      بنية Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      پایله = Curl_cf_capsule_ورننباسه_after(cf, معلومات);
      إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "adding capsule filter failed -> %d",
                    (صحيح)پایله);
        أرجع پایله;
      }
      پایله = Curl_cf_quic_ورننباسه_after(cf, معلومات, origin, peer);
      إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "adding QUIC filter failed -> %d", (صحيح)پایله);
        أرجع پایله;
      }
      CURL_TRC_CF(معلومات, cf, "added QUIC filter for origin");
    }
    وإلا
#د_که_پای /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#که_تعریف_شوی USE_SSL
    إذا((ctx->ssl_ډېرپېښ == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_ډېرپېښ != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
      إذا(cf->conn->بټونه.origin_is_proxy) {
        پایله = Curl_cf_ssl_proxy_ورننباسه_after(cf, معلومات, cf->conn->origin);
      }
      وإلا
#د_که_پای
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        پایله = Curl_cf_ssl_ورننباسه_after(cf, معلومات,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "adding SSL filter for origin failed -> %d",
                    (صحيح)پایله);
        أرجع پایله;
      }
      CURL_TRC_CF(معلومات, cf, "added SSL filter for origin");
    }
#د_که_پای /* USE_SSL */
    ctx->state = CF_تیاری_CNNCT_SSL;
  }
  أرجع پایله;
}

ساكن CURLcode cf_تیاری_connect_ګامونه(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       bool *بشپړ)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(معلومات, cf->sockindex, "cf_setup_connect"));

  إذا(cf->بل && !cf->بل->connected) {
    پایله = Curl_conn_cf_connect(cf->بل, معلومات, بشپړ);
    إذا(پایله || !*بشپړ)
      أرجع پایله;
  }

  پایله = cf_تیاری_جمع_کړه_ip_happy(cf, معلومات);
  إذا(پایله)
    أرجع پایله;
  إذا(!cf->بل || !cf->بل->connected)
    اذهب connect_sub_chain;

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  پایله = cf_تیاری_جمع_کړه_socks(cf, معلومات);
  إذا(پایله)
    أرجع پایله;
  إذا(!cf->بل || !cf->بل->connected)
    اذهب connect_sub_chain;

#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  پایله = cf_تیاری_جمع_کړه_http_proxy(cf, معلومات);
  إذا(پایله)
    أرجع پایله;
  إذا(!cf->بل || !cf->بل->connected)
    اذهب connect_sub_chain;
#د_که_پای /* !CURL_DISABLE_HTTP */

  پایله = cf_تیاری_جمع_کړه_haproxy(cf, معلومات);
  إذا(پایله)
    أرجع پایله;
  إذا(!cf->بل || !cf->بل->connected)
    اذهب connect_sub_chain;
#د_که_پای /* !CURL_DISABLE_PROXY */

  پایله = cf_تیاری_جمع_کړه_origin_filters(cf, معلومات);
  إذا(پایله)
    أرجع پایله;
  إذا(!cf->بل || !cf->بل->connected)
    اذهب connect_sub_chain;

  ctx->state = CF_تیاری_بشپړ;
  cf->connected = TRUE;
  *بشپړ = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_تیاری_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *بشپړ)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode پایله;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  پایله = cf_تیاری_connect_ګامونه(cf, معلومات, بشپړ);

  إذا(پایله == CURLE_AGAIN) {
    ++ctx->retry_شمېر;
    إذا(ctx->retry_شمېر > 5) /* arbitrary limit, better just timeout? */
      أرجع CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(معلومات, cf, "retrying connect, %d. time", ctx->retry_شمېر);
    Curl_conn_cf_discard_chain(&cf->بل, معلومات);
    ctx->state = CF_تیاری_INIT;
    اذهب retry;
  }
  أرجع پایله;
}

ساكن فراغ cf_تیاری_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  curlx_safefree(ctx);
}

بنية Curl_cftype Curl_cft_تیاری = {
  "SETUP",
  CF_TYPE_تیاری,
  CURL_LOG_LVL_NONE,
  cf_تیاری_destroy,
  cf_تیاری_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_تیاری_create(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *معلومات,
                                uint8_t transport,
                                صحيح ssl_ډېرپېښ)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_تیاری_ctx *ctx;
  CURLcode پایله = CURLE_OK;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  ctx->state = CF_تیاری_INIT;
  ctx->ssl_ډېرپېښ = ssl_ډېرپېښ;
  ctx->transport = transport;

  پایله = Curl_cf_create(&cf, &Curl_cft_تیاری, ctx);
  إذا(پایله)
    اذهب بهر;
  ctx = عدم;

بهر:
  *pcf = پایله ? عدم : cf;
  إذا(ctx) {
    curlx_free(ctx);
  }
  أرجع پایله;
}

CURLcode Curl_cf_تیاری_جمع_کړه(بنية Curl_easy *معلومات,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحيح ssl_ډېرپېښ)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(معلومات);
  پایله = cf_تیاری_create(&cf, معلومات, transport, ssl_ډېرپېښ);
  إذا(پایله)
    اذهب بهر;
  Curl_conn_cf_جمع_کړه(معلومات, conn, sockindex, cf);
بهر:
  أرجع پایله;
}

CURLcode Curl_cf_تیاری_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *معلومات,
                                    uint8_t transport,
                                    صحيح ssl_ډېرپېښ)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;

  DEBUGASSERT(معلومات);
  پایله = cf_تیاری_create(&cf, معلومات, transport, ssl_ډېرپېښ);
  إذا(پایله)
    اذهب بهر;
  Curl_conn_cf_ورننباسه_after(cf_at, cf);
بهر:
  أرجع پایله;
}
