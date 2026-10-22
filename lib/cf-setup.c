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
  CF_راهاندازی_INIT,
  CF_راهاندازی_CNNCT_EYEBALLS,
  CF_راهاندازی_CNNCT_SOCKS,
  CF_راهاندازی_CNNCT_HTTP_PROXY,
  CF_راهاندازی_CNNCT_HAPROXY,
  CF_راهاندازی_CNNCT_SSL,
  CF_راهاندازی_انجامشده
} cf_راهاندازی_state;

بنية cf_راهاندازی_ctx {
  cf_راهاندازی_state state;
  صحيح ssl_نما;
  uint8_t transport;
  uint8_t retry_شمار;
};

#اگر_تعریف_نشده CURL_DISABLE_PROXY

ساكن CURLcode cf_راهاندازی_جمع_کن_haproxy(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(ctx->state < CF_راهاندازی_CNNCT_HAPROXY) {
    إذا(داده->مجموعه.haproxyprotocol) {
      إذا(ctx->transport == TRNSPRT_QUIC) {
        failf(داده, "haproxy protocol does not support QUIC");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
      نتیجه = Curl_cf_haproxy_درج_after(cf, داده);
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "adding HAPROXY filter failed -> %d",
                    (صحيح)نتیجه);
        أرجع نتیجه;
      }
      CURL_TRC_CF(داده, cf, "added HAPROXY filter");
    }
    ctx->state = CF_راهاندازی_CNNCT_HAPROXY;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_راهاندازی_جمع_کن_socks(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  إذا(ctx->state < CF_راهاندازی_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    بنية Curl_peer *دومی_peer;

    إذا(cf->conn->http_proxy.peer)
      دومی_peer = cf->conn->http_proxy.peer;
    وإلا
      دومی_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    إذا(!دومی_peer)
      أرجع CURLE_FAILED_INIT;

    نتیجه = Curl_cf_socks_proxy_درج_after(
      cf, داده, دومی_peer, cf->conn->ip_نسخه,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "adding SOCKS filter failed -> %d", (صحيح)نتیجه);
      أرجع نتیجه;
    }

    CURL_TRC_CF(داده, cf, "added SOCKS filter to %s:%u",
                دومی_peer->hostname, دومی_peer->پورت);
    ctx->state = CF_راهاندازی_CNNCT_SOCKS;
  }
  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_HTTP
ساكن CURLcode cf_راهاندازی_جمع_کن_http_proxy(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(ctx->state < CF_راهاندازی_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->بیتها.origin_is_proxy) {
    بنية Curl_peer *peer = cf->conn->http_proxy.peer;
    بنية Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#اگر_تعریف_شده USE_SSL
    إذا(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      نتیجه = Curl_cf_ssl_proxy_درج_after(
        cf, داده, cf->conn->http_proxy.peer);
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (صحيح)نتیجه);
        أرجع نتیجه;
      }
      CURL_TRC_CF(داده, cf, "added SSL filter for HTTP proxy");
    }
#پایان_اگر /* USE_SSL */

    نتیجه = Curl_cf_http_proxy_درج_after(
      cf, داده, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (صحيح)نتیجه);
      أرجع نتیجه;
    }
    CURL_TRC_CF(داده, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_راهاندازی_CNNCT_HTTP_PROXY;
  }
  أرجع نتیجه;
}
#پایان_اگر /* !CURL_DISABLE_HTTP */
#پایان_اگر /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ساكن بنية Curl_peer *conn_get_اولی_origin(بنية connectdata *conn,
                                             int8_t sockindex)
{
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#پایان_اگر
  أرجع (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ساكن CURLcode cf_راهاندازی_جمع_کن_ip_happy(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(ctx->state < CF_راهاندازی_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    بنية Curl_peer *اولی_origin =
      conn_get_اولی_origin(cf->conn, cf->sockindex);
    بنية Curl_peer *اولی_peer =
      Curl_conn_get_اولی_peer(cf->conn, cf->sockindex);
    بنية Curl_peer *tunnel_peer = عدم;
    uint8_t اولی_transport = ctx->transport;

    إذا(!اولی_peer)
      أرجع CURLE_FAILED_INIT;

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    إذا(cf->conn->http_proxy.peer && !cf->conn->بیتها.origin_is_proxy) {
      اولی_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      إذا((اولی_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(داده, "HTTP/3 proxy not possible via SOCKS");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#پایان_اگر /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    نتیجه = Curl_cf_ip_happy_درج_after(cf, داده, اولی_origin, اولی_peer,
                                           اولی_transport,
                                           tunnel_peer, ctx->transport);
    إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "adding happy eyeballs failed -> %d", (صحيح)نتیجه);
      أرجع نتیجه;
    }

    إذا(tunnel_peer && (اولی_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(داده, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  اولی_peer->hostname, اولی_peer->پورت);
      ctx->state = CF_راهاندازی_CNNCT_HTTP_PROXY;
    }
    وإلا {
      CURL_TRC_CF(داده, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  اولی_peer->hostname, اولی_peer->پورت);
      ctx->state = CF_راهاندازی_CNNCT_EYEBALLS;
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_راهاندازی_جمع_کن_origin_filters(بنية Curl_cfilter *cf,
                                            بنية Curl_easy *داده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)داده; /* not used in all builds */
  إذا(ctx->state < CF_راهاندازی_CNNCT_SSL) {
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    إذا(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->بیتها.origin_is_proxy) {
      بنية Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      بنية Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      نتیجه = Curl_cf_capsule_درج_after(cf, داده);
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "adding capsule filter failed -> %d",
                    (صحيح)نتیجه);
        أرجع نتیجه;
      }
      نتیجه = Curl_cf_quic_درج_after(cf, داده, origin, peer);
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "adding QUIC filter failed -> %d", (صحيح)نتیجه);
        أرجع نتیجه;
      }
      CURL_TRC_CF(داده, cf, "added QUIC filter for origin");
    }
    وإلا
#پایان_اگر /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#اگر_تعریف_شده USE_SSL
    إذا((ctx->ssl_نما == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_نما != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#اگر_تعریف_نشده CURL_DISABLE_PROXY
      إذا(cf->conn->بیتها.origin_is_proxy) {
        نتیجه = Curl_cf_ssl_proxy_درج_after(cf, داده, cf->conn->origin);
      }
      وإلا
#پایان_اگر
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        نتیجه = Curl_cf_ssl_درج_after(cf, داده,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "adding SSL filter for origin failed -> %d",
                    (صحيح)نتیجه);
        أرجع نتیجه;
      }
      CURL_TRC_CF(داده, cf, "added SSL filter for origin");
    }
#پایان_اگر /* USE_SSL */
    ctx->state = CF_راهاندازی_CNNCT_SSL;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_راهاندازی_connect_گامها(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       bool *انجامشده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(داده, cf->sockindex, "cf_setup_connect"));

  إذا(cf->بعدی && !cf->بعدی->connected) {
    نتیجه = Curl_conn_cf_connect(cf->بعدی, داده, انجامشده);
    إذا(نتیجه || !*انجامشده)
      أرجع نتیجه;
  }

  نتیجه = cf_راهاندازی_جمع_کن_ip_happy(cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(!cf->بعدی || !cf->بعدی->connected)
    اذهب connect_sub_chain;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  نتیجه = cf_راهاندازی_جمع_کن_socks(cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(!cf->بعدی || !cf->بعدی->connected)
    اذهب connect_sub_chain;

#اگر_تعریف_نشده CURL_DISABLE_HTTP
  نتیجه = cf_راهاندازی_جمع_کن_http_proxy(cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(!cf->بعدی || !cf->بعدی->connected)
    اذهب connect_sub_chain;
#پایان_اگر /* !CURL_DISABLE_HTTP */

  نتیجه = cf_راهاندازی_جمع_کن_haproxy(cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(!cf->بعدی || !cf->بعدی->connected)
    اذهب connect_sub_chain;
#پایان_اگر /* !CURL_DISABLE_PROXY */

  نتیجه = cf_راهاندازی_جمع_کن_origin_filters(cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(!cf->بعدی || !cf->بعدی->connected)
    اذهب connect_sub_chain;

  ctx->state = CF_راهاندازی_انجامشده;
  cf->connected = TRUE;
  *انجامشده = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_راهاندازی_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 bool *انجامشده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;
  CURLcode نتیجه;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  نتیجه = cf_راهاندازی_connect_گامها(cf, داده, انجامشده);

  إذا(نتیجه == CURLE_AGAIN) {
    ++ctx->retry_شمار;
    إذا(ctx->retry_شمار > 5) /* arbitrary limit, better just timeout? */
      أرجع CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(داده, cf, "retrying connect, %d. time", ctx->retry_شمار);
    Curl_conn_cf_discard_chain(&cf->بعدی, داده);
    ctx->state = CF_راهاندازی_INIT;
    اذهب retry;
  }
  أرجع نتیجه;
}

ساكن فراغ cf_راهاندازی_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_راهاندازی_ctx *ctx = cf->ctx;

  CURL_TRC_CF(داده, cf, "destroy");
  curlx_safefree(ctx);
}

بنية Curl_cftype Curl_cft_راهاندازی = {
  "SETUP",
  CF_TYPE_راهاندازی,
  CURL_LOG_LVL_NONE,
  cf_راهاندازی_destroy,
  cf_راهاندازی_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_داده_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_راهاندازی_create(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *داده,
                                uint8_t transport,
                                صحيح ssl_نما)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_راهاندازی_ctx *ctx;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)داده;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  ctx->state = CF_راهاندازی_INIT;
  ctx->ssl_نما = ssl_نما;
  ctx->transport = transport;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_راهاندازی, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  ctx = عدم;

بیرون:
  *pcf = نتیجه ? عدم : cf;
  إذا(ctx) {
    curlx_free(ctx);
  }
  أرجع نتیجه;
}

CURLcode Curl_cf_راهاندازی_جمع_کن(بنية Curl_easy *داده,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحيح ssl_نما)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(داده);
  نتیجه = cf_راهاندازی_create(&cf, داده, transport, ssl_نما);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_جمع_کن(داده, conn, sockindex, cf);
بیرون:
  أرجع نتیجه;
}

CURLcode Curl_cf_راهاندازی_درج_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *داده,
                                    uint8_t transport,
                                    صحيح ssl_نما)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  نتیجه = cf_راهاندازی_create(&cf, داده, transport, ssl_نما);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_درج_after(cf_at, cf);
بیرون:
  أرجع نتیجه;
}
