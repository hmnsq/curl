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
  CF_تياري_INIT,
  CF_تياري_CNNCT_EYEBALLS,
  CF_تياري_CNNCT_SOCKS,
  CF_تياري_CNNCT_HTTP_PROXY,
  CF_تياري_CNNCT_HAPROXY,
  CF_تياري_CNNCT_SSL,
  CF_تياري_پورو
} cf_تياري_state;

بنية cf_تياري_ctx {
  cf_تياري_state state;
  صحيح ssl_گهڻائي;
  uint8_t transport;
  uint8_t retry_ڳڻپ;
};

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY

ساكن CURLcode cf_تياري_جوڙيو_haproxy(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(ctx->state < CF_تياري_CNNCT_HAPROXY) {
    إذا(معلومات->سيٽ.haproxyprotocol) {
      إذا(ctx->transport == TRNSPRT_QUIC) {
        failf(معلومات, "haproxy protocol does not support QUIC");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
      نتيجو = Curl_cf_haproxy_وجهو_after(cf, معلومات);
      إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "adding HAPROXY filter failed -> %d",
                    (صحيح)نتيجو);
        أرجع نتيجو;
      }
      CURL_TRC_CF(معلومات, cf, "added HAPROXY filter");
    }
    ctx->state = CF_تياري_CNNCT_HAPROXY;
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_تياري_جوڙيو_socks(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  إذا(ctx->state < CF_تياري_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    بنية Curl_peer *ٻيو_peer;

    إذا(cf->conn->http_proxy.peer)
      ٻيو_peer = cf->conn->http_proxy.peer;
    وإلا
      ٻيو_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    إذا(!ٻيو_peer)
      أرجع CURLE_FAILED_INIT;

    نتيجو = Curl_cf_socks_proxy_وجهو_after(
      cf, معلومات, ٻيو_peer, cf->conn->ip_نسخو,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    إذا(نتيجو) {
      CURL_TRC_CF(معلومات, cf, "adding SOCKS filter failed -> %d", (صحيح)نتيجو);
      أرجع نتيجو;
    }

    CURL_TRC_CF(معلومات, cf, "added SOCKS filter to %s:%u",
                ٻيو_peer->hostname, ٻيو_peer->پورٽ);
    ctx->state = CF_تياري_CNNCT_SOCKS;
  }
  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
ساكن CURLcode cf_تياري_جوڙيو_http_proxy(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(ctx->state < CF_تياري_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->بٽ.origin_is_proxy) {
    بنية Curl_peer *peer = cf->conn->http_proxy.peer;
    بنية Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      نتيجو = Curl_cf_ssl_proxy_وجهو_after(
        cf, معلومات, cf->conn->http_proxy.peer);
      إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (صحيح)نتيجو);
        أرجع نتيجو;
      }
      CURL_TRC_CF(معلومات, cf, "added SSL filter for HTTP proxy");
    }
#جيڪڏهن_پورو /* USE_SSL */

    نتيجو = Curl_cf_http_proxy_وجهو_after(
      cf, معلومات, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    إذا(نتيجو) {
      CURL_TRC_CF(معلومات, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (صحيح)نتيجو);
      أرجع نتيجو;
    }
    CURL_TRC_CF(معلومات, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_تياري_CNNCT_HTTP_PROXY;
  }
  أرجع نتيجو;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */
#جيڪڏهن_پورو /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ساكن بنية Curl_peer *conn_get_پهريون_origin(بنية connectdata *conn,
                                             int8_t sockindex)
{
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer)
    أرجع conn->socks_proxy.peer;
  إذا(conn->http_proxy.peer)
    أرجع conn->http_proxy.peer;
#جيڪڏهن_پورو
  أرجع (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ساكن CURLcode cf_تياري_جوڙيو_ip_happy(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(ctx->state < CF_تياري_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    بنية Curl_peer *پهريون_origin =
      conn_get_پهريون_origin(cf->conn, cf->sockindex);
    بنية Curl_peer *پهريون_peer =
      Curl_conn_get_پهريون_peer(cf->conn, cf->sockindex);
    بنية Curl_peer *tunnel_peer = عدم;
    uint8_t پهريون_transport = ctx->transport;

    إذا(!پهريون_peer)
      أرجع CURLE_FAILED_INIT;

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    إذا(cf->conn->http_proxy.peer && !cf->conn->بٽ.origin_is_proxy) {
      پهريون_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      إذا((پهريون_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(معلومات, "HTTP/3 proxy not possible via SOCKS");
        أرجع CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    نتيجو = Curl_cf_ip_happy_وجهو_after(cf, معلومات, پهريون_origin, پهريون_peer,
                                           پهريون_transport,
                                           tunnel_peer, ctx->transport);
    إذا(نتيجو) {
      CURL_TRC_CF(معلومات, cf, "adding happy eyeballs failed -> %d", (صحيح)نتيجو);
      أرجع نتيجو;
    }

    إذا(tunnel_peer && (پهريون_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(معلومات, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  پهريون_peer->hostname, پهريون_peer->پورٽ);
      ctx->state = CF_تياري_CNNCT_HTTP_PROXY;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  پهريون_peer->hostname, پهريون_peer->پورٽ);
      ctx->state = CF_تياري_CNNCT_EYEBALLS;
    }
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_تياري_جوڙيو_origin_filters(بنية Curl_cfilter *cf,
                                            بنية Curl_easy *معلومات)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)معلومات; /* not used in all builds */
  إذا(ctx->state < CF_تياري_CNNCT_SSL) {
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    إذا(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->بٽ.origin_is_proxy) {
      بنية Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      بنية Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      نتيجو = Curl_cf_capsule_وجهو_after(cf, معلومات);
      إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "adding capsule filter failed -> %d",
                    (صحيح)نتيجو);
        أرجع نتيجو;
      }
      نتيجو = Curl_cf_quic_وجهو_after(cf, معلومات, origin, peer);
      إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "adding QUIC filter failed -> %d", (صحيح)نتيجو);
        أرجع نتيجو;
      }
      CURL_TRC_CF(معلومات, cf, "added QUIC filter for origin");
    }
    وإلا
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#جيڪڏهن_وصف_ٿيل USE_SSL
    إذا((ctx->ssl_گهڻائي == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_گهڻائي != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
      إذا(cf->conn->بٽ.origin_is_proxy) {
        نتيجو = Curl_cf_ssl_proxy_وجهو_after(cf, معلومات, cf->conn->origin);
      }
      وإلا
#جيڪڏهن_پورو
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        نتيجو = Curl_cf_ssl_وجهو_after(cf, معلومات,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "adding SSL filter for origin failed -> %d",
                    (صحيح)نتيجو);
        أرجع نتيجو;
      }
      CURL_TRC_CF(معلومات, cf, "added SSL filter for origin");
    }
#جيڪڏهن_پورو /* USE_SSL */
    ctx->state = CF_تياري_CNNCT_SSL;
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_تياري_connect_قدمن(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       bool *پورو)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(معلومات, cf->sockindex, "cf_setup_connect"));

  إذا(cf->ايندڙ && !cf->ايندڙ->connected) {
    نتيجو = Curl_conn_cf_connect(cf->ايندڙ, معلومات, پورو);
    إذا(نتيجو || !*پورو)
      أرجع نتيجو;
  }

  نتيجو = cf_تياري_جوڙيو_ip_happy(cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(!cf->ايندڙ || !cf->ايندڙ->connected)
    اذهب connect_sub_chain;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  نتيجو = cf_تياري_جوڙيو_socks(cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(!cf->ايندڙ || !cf->ايندڙ->connected)
    اذهب connect_sub_chain;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  نتيجو = cf_تياري_جوڙيو_http_proxy(cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(!cf->ايندڙ || !cf->ايندڙ->connected)
    اذهب connect_sub_chain;
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */

  نتيجو = cf_تياري_جوڙيو_haproxy(cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(!cf->ايندڙ || !cf->ايندڙ->connected)
    اذهب connect_sub_chain;
#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY */

  نتيجو = cf_تياري_جوڙيو_origin_filters(cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(!cf->ايندڙ || !cf->ايندڙ->connected)
    اذهب connect_sub_chain;

  ctx->state = CF_تياري_پورو;
  cf->connected = TRUE;
  *پورو = TRUE;
  أرجع CURLE_OK;
}

ساكن CURLcode cf_تياري_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *پورو)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;
  CURLcode نتيجو;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  نتيجو = cf_تياري_connect_قدمن(cf, معلومات, پورو);

  إذا(نتيجو == CURLE_AGAIN) {
    ++ctx->retry_ڳڻپ;
    إذا(ctx->retry_ڳڻپ > 5) /* arbitrary limit, better just timeout? */
      أرجع CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(معلومات, cf, "retrying connect, %d. time", ctx->retry_ڳڻپ);
    Curl_conn_cf_discard_chain(&cf->ايندڙ, معلومات);
    ctx->state = CF_تياري_INIT;
    اذهب retry;
  }
  أرجع نتيجو;
}

ساكن فراغ cf_تياري_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_تياري_ctx *ctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  curlx_safefree(ctx);
}

بنية Curl_cftype Curl_cft_تياري = {
  "SETUP",
  CF_TYPE_تياري,
  CURL_LOG_LVL_NONE,
  cf_تياري_destroy,
  cf_تياري_connect,
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

ساكن CURLcode cf_تياري_create(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *معلومات,
                                uint8_t transport,
                                صحيح ssl_گهڻائي)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_تياري_ctx *ctx;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  ctx->state = CF_تياري_INIT;
  ctx->ssl_گهڻائي = ssl_گهڻائي;
  ctx->transport = transport;

  نتيجو = Curl_cf_create(&cf, &Curl_cft_تياري, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;
  ctx = عدم;

ٻاهريون:
  *pcf = نتيجو ? عدم : cf;
  إذا(ctx) {
    curlx_free(ctx);
  }
  أرجع نتيجو;
}

CURLcode Curl_cf_تياري_جوڙيو(بنية Curl_easy *معلومات,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحيح ssl_گهڻائي)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(معلومات);
  نتيجو = cf_تياري_create(&cf, معلومات, transport, ssl_گهڻائي);
  إذا(نتيجو)
    اذهب ٻاهريون;
  Curl_conn_cf_جوڙيو(معلومات, conn, sockindex, cf);
ٻاهريون:
  أرجع نتيجو;
}

CURLcode Curl_cf_تياري_وجهو_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *معلومات,
                                    uint8_t transport,
                                    صحيح ssl_گهڻائي)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات);
  نتيجو = cf_تياري_create(&cf, معلومات, transport, ssl_گهڻائي);
  إذا(نتيجو)
    اذهب ٻاهريون;
  Curl_conn_cf_وجهو_after(cf_at, cf);
ٻاهريون:
  أرجع نتيجو;
}
