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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-haproxy.h"
#అన్తర్భావ "cf-ip-happy.h"
#అన్తర్భావ "cf-setup.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "socks.h"
#అన్తర్భావ "vquic/cf-capsule.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "vtls/vtls.h"


రకనిర్వచనం గణన {
  CF_అమరిక_INIT,
  CF_అమరిక_CNNCT_EYEBALLS,
  CF_అమరిక_CNNCT_SOCKS,
  CF_అమరిక_CNNCT_HTTP_PROXY,
  CF_అమరిక_CNNCT_HAPROXY,
  CF_అమరిక_CNNCT_SSL,
  CF_అమరిక_పూర్తి
} cf_అమరిక_state;

నిర్మాణం cf_అమరిక_ctx {
  cf_అమరిక_state state;
  పూర్ణాంకం ssl_బహుళకం;
  uint8_t transport;
  uint8_t retry_లెక్క;
};

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

స్థిర CURLcode cf_అమరిక_కూడు_haproxy(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ctx->state < CF_అమరిక_CNNCT_HAPROXY) {
    ఒకవేళ(దత్తాంశం->సమితి.haproxyprotocol) {
      ఒకవేళ(ctx->transport == TRNSPRT_QUIC) {
        failf(దత్తాంశం, "haproxy protocol does not support QUIC");
        తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
      }
      ఫలితం = Curl_cf_haproxy_చొప్పించు_after(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "adding HAPROXY filter failed -> %d",
                    (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      CURL_TRC_CF(దత్తాంశం, cf, "added HAPROXY filter");
    }
    ctx->state = CF_అమరిక_CNNCT_HAPROXY;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_అమరిక_కూడు_socks(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(ctx->state < CF_అమరిక_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    నిర్మాణం Curl_peer *రెండవ_peer;

    ఒకవేళ(cf->conn->http_proxy.peer)
      రెండవ_peer = cf->conn->http_proxy.peer;
    లేకపోతే
      రెండవ_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    ఒకవేళ(!రెండవ_peer)
      తిరిగి CURLE_FAILED_INIT;

    ఫలితం = Curl_cf_socks_proxy_చొప్పించు_after(
      cf, దత్తాంశం, రెండవ_peer, cf->conn->ip_సంస్కరణ,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "adding SOCKS filter failed -> %d", (పూర్ణాంకం)ఫలితం);
      తిరిగి ఫలితం;
    }

    CURL_TRC_CF(దత్తాంశం, cf, "added SOCKS filter to %s:%u",
                రెండవ_peer->hostname, రెండవ_peer->పోర్టు);
    ctx->state = CF_అమరిక_CNNCT_SOCKS;
  }
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
స్థిర CURLcode cf_అమరిక_కూడు_http_proxy(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ctx->state < CF_అమరిక_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->బిట్లు.origin_is_proxy) {
    నిర్మాణం Curl_peer *peer = cf->conn->http_proxy.peer;
    నిర్మాణం Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#నిర్వచితమైతే USE_SSL
    ఒకవేళ(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      ఫలితం = Curl_cf_ssl_proxy_చొప్పించు_after(
        cf, దత్తాంశం, cf->conn->http_proxy.peer);
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      CURL_TRC_CF(దత్తాంశం, cf, "added SSL filter for HTTP proxy");
    }
#అయితే_ముగింపు /* USE_SSL */

    ఫలితం = Curl_cf_http_proxy_చొప్పించు_after(
      cf, దత్తాంశం, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (పూర్ణాంకం)ఫలితం);
      తిరిగి ఫలితం;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_అమరిక_CNNCT_HTTP_PROXY;
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
#అయితే_ముగింపు /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
స్థిర నిర్మాణం Curl_peer *conn_get_మొదటి_origin(నిర్మాణం connectdata *conn,
                                             int8_t sockindex)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(conn->socks_proxy.peer)
    తిరిగి conn->socks_proxy.peer;
  ఒకవేళ(conn->http_proxy.peer)
    తిరిగి conn->http_proxy.peer;
#అయితే_ముగింపు
  తిరిగి (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

స్థిర CURLcode cf_అమరిక_కూడు_ip_happy(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ctx->state < CF_అమరిక_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    నిర్మాణం Curl_peer *మొదటి_origin =
      conn_get_మొదటి_origin(cf->conn, cf->sockindex);
    నిర్మాణం Curl_peer *మొదటి_peer =
      Curl_conn_get_మొదటి_peer(cf->conn, cf->sockindex);
    నిర్మాణం Curl_peer *tunnel_peer = ఖాళీ;
    uint8_t మొదటి_transport = ctx->transport;

    ఒకవేళ(!మొదటి_peer)
      తిరిగి CURLE_FAILED_INIT;

#ఒకవేళ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    ఒకవేళ(cf->conn->http_proxy.peer && !cf->conn->బిట్లు.origin_is_proxy) {
      మొదటి_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      ఒకవేళ((మొదటి_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(దత్తాంశం, "HTTP/3 proxy not possible via SOCKS");
        తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#అయితే_ముగింపు /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    ఫలితం = Curl_cf_ip_happy_చొప్పించు_after(cf, దత్తాంశం, మొదటి_origin, మొదటి_peer,
                                           మొదటి_transport,
                                           tunnel_peer, ctx->transport);
    ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "adding happy eyeballs failed -> %d", (పూర్ణాంకం)ఫలితం);
      తిరిగి ఫలితం;
    }

    ఒకవేళ(tunnel_peer && (మొదటి_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(దత్తాంశం, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  మొదటి_peer->hostname, మొదటి_peer->పోర్టు);
      ctx->state = CF_అమరిక_CNNCT_HTTP_PROXY;
    }
    లేకపోతే {
      CURL_TRC_CF(దత్తాంశం, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  మొదటి_peer->hostname, మొదటి_peer->పోర్టు);
      ctx->state = CF_అమరిక_CNNCT_EYEBALLS;
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_అమరిక_కూడు_origin_filters(నిర్మాణం Curl_cfilter *cf,
                                            నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)దత్తాంశం; /* not used in all builds */
  ఒకవేళ(ctx->state < CF_అమరిక_CNNCT_SSL) {
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    ఒకవేళ(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->బిట్లు.origin_is_proxy) {
      నిర్మాణం Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      నిర్మాణం Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      ఫలితం = Curl_cf_capsule_చొప్పించు_after(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "adding capsule filter failed -> %d",
                    (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      ఫలితం = Curl_cf_quic_చొప్పించు_after(cf, దత్తాంశం, origin, peer);
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "adding QUIC filter failed -> %d", (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      CURL_TRC_CF(దత్తాంశం, cf, "added QUIC filter for origin");
    }
    లేకపోతే
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#నిర్వచితమైతే USE_SSL
    ఒకవేళ((ctx->ssl_బహుళకం == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_బహుళకం != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
      ఒకవేళ(cf->conn->బిట్లు.origin_is_proxy) {
        ఫలితం = Curl_cf_ssl_proxy_చొప్పించు_after(cf, దత్తాంశం, cf->conn->origin);
      }
      లేకపోతే
#అయితే_ముగింపు
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        ఫలితం = Curl_cf_ssl_చొప్పించు_after(cf, దత్తాంశం,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "adding SSL filter for origin failed -> %d",
                    (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      CURL_TRC_CF(దత్తాంశం, cf, "added SSL filter for origin");
    }
#అయితే_ముగింపు /* USE_SSL */
    ctx->state = CF_అమరిక_CNNCT_SSL;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_అమరిక_connect_అడుగులు(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool *పూర్తి)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(దత్తాంశం, cf->sockindex, "cf_setup_connect"));

  ఒకవేళ(cf->తదుపరి && !cf->తదుపరి->connected) {
    ఫలితం = Curl_conn_cf_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  ఫలితం = cf_అమరిక_కూడు_ip_happy(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->connected)
    వెళ్ళు connect_sub_chain;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఫలితం = cf_అమరిక_కూడు_socks(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->connected)
    వెళ్ళు connect_sub_chain;

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  ఫలితం = cf_అమరిక_కూడు_http_proxy(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->connected)
    వెళ్ళు connect_sub_chain;
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */

  ఫలితం = cf_అమరిక_కూడు_haproxy(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->connected)
    వెళ్ళు connect_sub_chain;
#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */

  ఫలితం = cf_అమరిక_కూడు_origin_filters(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->connected)
    వెళ్ళు connect_sub_chain;

  ctx->state = CF_అమరిక_పూర్తి;
  cf->connected = TRUE;
  *పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_అమరిక_connect(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool *పూర్తి)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  ఫలితం = cf_అమరిక_connect_అడుగులు(cf, దత్తాంశం, పూర్తి);

  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    ++ctx->retry_లెక్క;
    ఒకవేళ(ctx->retry_లెక్క > 5) /* arbitrary limit, better just timeout? */
      తిరిగి CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(దత్తాంశం, cf, "retrying connect, %d. time", ctx->retry_లెక్క);
    Curl_conn_cf_discard_chain(&cf->తదుపరి, దత్తాంశం);
    ctx->state = CF_అమరిక_INIT;
    వెళ్ళు retry;
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_అమరిక_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_అమరిక_ctx *ctx = cf->ctx;

  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  curlx_safefree(ctx);
}

నిర్మాణం Curl_cftype Curl_cft_అమరిక = {
  "SETUP",
  CF_TYPE_అమరిక,
  CURL_LOG_LVL_NONE,
  cf_అమరిక_destroy,
  cf_అమరిక_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

స్థిర CURLcode cf_అమరిక_create(నిర్మాణం Curl_cfilter **pcf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                uint8_t transport,
                                పూర్ణాంకం ssl_బహుళకం)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_అమరిక_ctx *ctx;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)దత్తాంశం;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ctx->state = CF_అమరిక_INIT;
  ctx->ssl_బహుళకం = ssl_బహుళకం;
  ctx->transport = transport;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_అమరిక, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ctx = ఖాళీ;

బహిర్గతం:
  *pcf = ఫలితం ? ఖాళీ : cf;
  ఒకవేళ(ctx) {
    curlx_free(ctx);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_అమరిక_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           పూర్ణాంకం ssl_బహుళకం)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(దత్తాంశం);
  ఫలితం = cf_అమరిక_create(&cf, దత్తాంశం, transport, ssl_బహుళకం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_కూడు(దత్తాంశం, conn, sockindex, cf);
బహిర్గతం:
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_అమరిక_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    uint8_t transport,
                                    పూర్ణాంకం ssl_బహుళకం)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  ఫలితం = cf_అమరిక_create(&cf, దత్తాంశం, transport, ssl_బహుళకం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_చొప్పించు_after(cf_at, cf);
బహిర్గతం:
  తిరిగి ఫలితం;
}
