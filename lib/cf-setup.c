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
  CF_تیاری_پورا
} cf_تیاری_state;

بنية cf_تیاری_ctx {
  cf_تیاری_state state;
  صحيح ssl_منوال;
  uint8_t transport;
  uint8_t retry_گنتی;
};

#جے_بنا_تعریف CURL_DISABLE_PROXY

ساكن CURLcode cf_تیاری_جوڑو_haproxy(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(ctx->state < CF_تیاری_CNNCT_HAPROXY) {
    إذا(انکڑے->سیٹ.haproxyprotocol) {
      إذا(ctx->transport == TRNSPRT_QUIC) {
        failf(انکڑے, "haproxy protocol does not support QUIC");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
      نتیجہ = Curl_cf_haproxy_داخل_کرو_after(cf, انکڑے);
      إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "adding HAPROXY filter failed -> %d",
                    (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      CURL_TRC_CF(انکڑے, cf, "added HAPROXY filter");
    }
    ctx->state = CF_تیاری_CNNCT_HAPROXY;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_تیاری_جوڑو_socks(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  إذا(ctx->state < CF_تیاری_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    بنية Curl_peer *دوجا_peer;

    إذا(cf->conn->http_proxy.peer)
      دوجا_peer = cf->conn->http_proxy.peer;
    وإلا
      دوجا_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    إذا(!دوجا_peer)
      أرجع CURLE_FAILED_INIT;

    نتیجہ = Curl_cf_socks_proxy_داخل_کرو_after(
      cf, انکڑے, دوجا_peer, cf->conn->ip_نسخہ,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "adding SOCKS filter failed -> %d", (صحيح)نتیجہ);
      أرجع نتیجہ;
    }

    CURL_TRC_CF(انکڑے, cf, "added SOCKS filter to %s:%u",
                دوجا_peer->hostname, دوجا_peer->پورٹ);
    ctx->state = CF_تیاری_CNNCT_SOCKS;
  }
  أرجع نتیجہ;
}

#جے_بنا_تعریف CURL_DISABLE_HTTP
ساكن CURLcode cf_تیاری_جوڑو_http_proxy(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(ctx->state < CF_تیاری_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->بٹ.origin_is_proxy) {
    بنية Curl_peer *peer = cf->conn->http_proxy.peer;
    بنية Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#جے_تعریف_شدہ USE_SSL
    إذا(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      نتیجہ = Curl_cf_ssl_proxy_داخل_کرو_after(
        cf, انکڑے, cf->conn->http_proxy.peer);
      إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      CURL_TRC_CF(انکڑے, cf, "added SSL filter for HTTP proxy");
    }
#جے_انت /* USE_SSL */

    نتیجہ = Curl_cf_http_proxy_داخل_کرو_after(
      cf, انکڑے, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (صحيح)نتیجہ);
      أرجع نتیجہ;
    }
    CURL_TRC_CF(انکڑے, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_تیاری_CNNCT_HTTP_PROXY;
  }
  أرجع نتیجہ;
}
#جے_انت /* !CURL_DISABLE_HTTP */
#جے_انت /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ساكن بنية Curl_peer *conn_get_پہلا_origin(بنية connectdata *conn,
                                             int8_t sockindex)
{
#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#جے_انت
  أرجع (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ساكن CURLcode cf_تیاری_جوڑو_ip_happy(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(ctx->state < CF_تیاری_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    بنية Curl_peer *پہلا_origin =
      conn_get_پہلا_origin(cf->conn, cf->sockindex);
    بنية Curl_peer *پہلا_peer =
      Curl_conn_get_پہلا_peer(cf->conn, cf->sockindex);
    بنية Curl_peer *tunnel_peer = عدم;
    uint8_t پہلا_transport = ctx->transport;

    إذا(!پہلا_peer)
      أرجع CURLE_FAILED_INIT;

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    إذا(cf->conn->http_proxy.peer && !cf->conn->بٹ.origin_is_proxy) {
      پہلا_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      إذا((پہلا_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(انکڑے, "HTTP/3 proxy not possible via SOCKS");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#جے_انت /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    نتیجہ = Curl_cf_ip_happy_داخل_کرو_after(cf, انکڑے, پہلا_origin, پہلا_peer,
                                           پہلا_transport,
                                           tunnel_peer, ctx->transport);
    إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "adding happy eyeballs failed -> %d", (صحيح)نتیجہ);
      أرجع نتیجہ;
    }

    إذا(tunnel_peer && (پہلا_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(انکڑے, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  پہلا_peer->hostname, پہلا_peer->پورٹ);
      ctx->state = CF_تیاری_CNNCT_HTTP_PROXY;
    }
    وإلا {
      CURL_TRC_CF(انکڑے, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  پہلا_peer->hostname, پہلا_peer->پورٹ);
      ctx->state = CF_تیاری_CNNCT_EYEBALLS;
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_تیاری_جوڑو_origin_filters(بنية Curl_cfilter *cf,
                                            بنية Curl_easy *انکڑے)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)انکڑے; /* not used in all builds */
  إذا(ctx->state < CF_تیاری_CNNCT_SSL) {
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    إذا(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->بٹ.origin_is_proxy) {
      بنية Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      بنية Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      نتیجہ = Curl_cf_capsule_داخل_کرو_after(cf, انکڑے);
      إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "adding capsule filter failed -> %d",
                    (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      نتیجہ = Curl_cf_quic_داخل_کرو_after(cf, انکڑے, origin, peer);
      إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "adding QUIC filter failed -> %d", (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      CURL_TRC_CF(انکڑے, cf, "added QUIC filter for origin");
    }
    وإلا
#جے_انت /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#جے_تعریف_شدہ USE_SSL
    إذا((ctx->ssl_منوال == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_منوال != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#جے_بنا_تعریف CURL_DISABLE_PROXY
      إذا(cf->conn->بٹ.origin_is_proxy) {
        نتیجہ = Curl_cf_ssl_proxy_داخل_کرو_after(cf, انکڑے, cf->conn->origin);
      }
      وإلا
#جے_انت
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        نتیجہ = Curl_cf_ssl_داخل_کرو_after(cf, انکڑے,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "adding SSL filter for origin failed -> %d",
                    (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      CURL_TRC_CF(انکڑے, cf, "added SSL filter for origin");
    }
#جے_انت /* USE_SSL */
    ctx->state = CF_تیاری_CNNCT_SSL;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_تیاری_connect_قدماں(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے,
                                       bool *پورا)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(انکڑے, cf->sockindex, "cf_setup_connect"));

  إذا(cf->اگلا && !cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, انکڑے, پورا);
    إذا(نتیجہ || !*پورا)
      أرجع نتیجہ;
  }

  نتیجہ = cf_تیاری_جوڑو_ip_happy(cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(!cf->اگلا || !cf->اگلا->connected)
    اذهب connect_sub_chain;

#جے_بنا_تعریف CURL_DISABLE_PROXY
  نتیجہ = cf_تیاری_جوڑو_socks(cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(!cf->اگلا || !cf->اگلا->connected)
    اذهب connect_sub_chain;

#جے_بنا_تعریف CURL_DISABLE_HTTP
  نتیجہ = cf_تیاری_جوڑو_http_proxy(cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(!cf->اگلا || !cf->اگلا->connected)
    اذهب connect_sub_chain;
#جے_انت /* !CURL_DISABLE_HTTP */

  نتیجہ = cf_تیاری_جوڑو_haproxy(cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(!cf->اگلا || !cf->اگلا->connected)
    اذهب connect_sub_chain;
#جے_انت /* !CURL_DISABLE_PROXY */

  نتیجہ = cf_تیاری_جوڑو_origin_filters(cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;
  إذا(!cf->اگلا || !cf->اگلا->connected)
    اذهب connect_sub_chain;

  ctx->state = CF_تیاری_پورا;
  cf->connected = TRUE;
  *پورا = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_تیاری_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 bool *پورا)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  نتیجہ = cf_تیاری_connect_قدماں(cf, انکڑے, پورا);

  إذا(نتیجہ == CURLE_AGAIN) {
    ++ctx->retry_گنتی;
    إذا(ctx->retry_گنتی > 5) /* arbitrary limit, better just timeout? */
      أرجع CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(انکڑے, cf, "retrying connect, %d. time", ctx->retry_گنتی);
    Curl_conn_cf_discard_chain(&cf->اگلا, انکڑے);
    ctx->state = CF_تیاری_INIT;
    اذهب retry;
  }
  أرجع نتیجہ;
}

ساكن فراغ cf_تیاری_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية cf_تیاری_ctx *ctx = cf->ctx;

  CURL_TRC_CF(انکڑے, cf, "destroy");
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
  Curl_cf_def_انکڑے_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_تیاری_create(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *انکڑے,
                                uint8_t transport,
                                صحيح ssl_منوال)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_تیاری_ctx *ctx;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)انکڑے;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  ctx->state = CF_تیاری_INIT;
  ctx->ssl_منوال = ssl_منوال;
  ctx->transport = transport;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_تیاری, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  ctx = عدم;

باہرلا:
  *pcf = نتیجہ ? عدم : cf;
  إذا(ctx) {
    curlx_free(ctx);
  }
  أرجع نتیجہ;
}

CURLcode Curl_cf_تیاری_جوڑو(بنية Curl_easy *انکڑے,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحيح ssl_منوال)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(انکڑے);
  نتیجہ = cf_تیاری_create(&cf, انکڑے, transport, ssl_منوال);
  إذا(نتیجہ)
    اذهب باہرلا;
  Curl_conn_cf_جوڑو(انکڑے, conn, sockindex, cf);
باہرلا:
  أرجع نتیجہ;
}

CURLcode Curl_cf_تیاری_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *انکڑے,
                                    uint8_t transport,
                                    صحيح ssl_منوال)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ;

  DEBUGASSERT(انکڑے);
  نتیجہ = cf_تیاری_create(&cf, انکڑے, transport, ssl_منوال);
  إذا(نتیجہ)
    اذهب باہرلا;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
باہرلا:
  أرجع نتیجہ;
}
