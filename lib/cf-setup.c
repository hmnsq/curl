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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-haproxy.h"
#অন্তর্ভাব "cf-ip-happy.h"
#অন্তর্ভাব "cf-setup.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "socks.h"
#অন্তর্ভাব "vquic/cf-capsule.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "vtls/vtls.h"


প্রকারলেখ গণনা {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

কাঠামো cf_setup_ctx {
  cf_setup_state state;
  পূর্ণসংখ্যা ssl_mode;
  uint8_t transport;
  uint8_t retry_গুনতি;
};

#ifndef CURL_DISABLE_PROXY

স্থির CURLcode cf_setup_add_haproxy(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    যদি(তথ্য->set.haproxyprotocol) {
      যদি(ctx->transport == TRNSPRT_QUIC) {
        failf(তথ্য, "haproxy protocol does not support QUIC");
        ফেরত CURLE_UNSUPPORTED_PROTOCOL;
      }
      ফলাফল = Curl_cf_haproxy_ঢোকাও_after(cf, তথ্য);
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "adding HAPROXY filter failed -> %d",
                    (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      CURL_TRC_CF(তথ্য, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_setup_add_socks(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  যদি(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    কাঠামো Curl_peer *second_peer;

    যদি(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    অন্যথা
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    যদি(!second_peer)
      ফেরত CURLE_FAILED_INIT;

    ফলাফল = Curl_cf_socks_proxy_ঢোকাও_after(
      cf, তথ্য, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "adding SOCKS filter failed -> %d", (পূর্ণসংখ্যা)ফলাফল);
      ফেরত ফলাফল;
    }

    CURL_TRC_CF(তথ্য, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_HTTP
স্থির CURLcode cf_setup_add_http_proxy(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    কাঠামো Curl_peer *peer = cf->conn->http_proxy.peer;
    কাঠামো Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    যদি(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      ফলাফল = Curl_cf_ssl_proxy_ঢোকাও_after(
        cf, তথ্য, cf->conn->http_proxy.peer);
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      CURL_TRC_CF(তথ্য, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    ফলাফল = Curl_cf_http_proxy_ঢোকাও_after(
      cf, তথ্য, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (পূর্ণসংখ্যা)ফলাফল);
      ফেরত ফলাফল;
    }
    CURL_TRC_CF(তথ্য, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  ফেরত ফলাফল;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
স্থির কাঠামো Curl_peer *conn_get_first_origin(কাঠামো connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  যদি(conn->socks_proxy.peer)
    ফেরত conn->socks_proxy.peer;
  যদি(conn->http_proxy.peer)
    ফেরত conn->http_proxy.peer;
#endif
  ফেরত (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

স্থির CURLcode cf_setup_add_ip_happy(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    কাঠামো Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    কাঠামো Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    কাঠামো Curl_peer *tunnel_peer = ফাঁকা;
    uint8_t first_transport = ctx->transport;

    যদি(!first_peer)
      ফেরত CURLE_FAILED_INIT;

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    যদি(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      যদি((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(তথ্য, "HTTP/3 proxy not possible via SOCKS");
        ফেরত CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    ফলাফল = Curl_cf_ip_happy_ঢোকাও_after(cf, তথ্য, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "adding happy eyeballs failed -> %d", (পূর্ণসংখ্যা)ফলাফল);
      ফেরত ফলাফল;
    }

    যদি(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(তথ্য, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    অন্যথা {
      CURL_TRC_CF(তথ্য, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_setup_add_origin_filters(কাঠামো Curl_cfilter *cf,
                                            কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)তথ্য; /* not used in all builds */
  যদি(ctx->state < CF_SETUP_CNNCT_SSL) {
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    যদি(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      কাঠামো Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      কাঠামো Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      ফলাফল = Curl_cf_capsule_ঢোকাও_after(cf, তথ্য);
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "adding capsule filter failed -> %d",
                    (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      ফলাফল = Curl_cf_quic_ঢোকাও_after(cf, তথ্য, origin, peer);
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "adding QUIC filter failed -> %d", (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      CURL_TRC_CF(তথ্য, cf, "added QUIC filter for origin");
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
        ফলাফল = Curl_cf_ssl_proxy_ঢোকাও_after(cf, তথ্য, cf->conn->origin);
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
        ফলাফল = Curl_cf_ssl_ঢোকাও_after(cf, তথ্য,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "adding SSL filter for origin failed -> %d",
                    (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      CURL_TRC_CF(তথ্য, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_setup_connect_steps(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       bool *done)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(তথ্য, cf->sockindex, "cf_setup_connect"));

  যদি(cf->পরবর্তী && !cf->পরবর্তী->connected) {
    ফলাফল = Curl_conn_cf_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  ফলাফল = cf_setup_add_ip_happy(cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(!cf->পরবর্তী || !cf->পরবর্তী->connected)
    যাও connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  ফলাফল = cf_setup_add_socks(cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(!cf->পরবর্তী || !cf->পরবর্তী->connected)
    যাও connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  ফলাফল = cf_setup_add_http_proxy(cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(!cf->পরবর্তী || !cf->পরবর্তী->connected)
    যাও connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  ফলাফল = cf_setup_add_haproxy(cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(!cf->পরবর্তী || !cf->পরবর্তী->connected)
    যাও connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  ফলাফল = cf_setup_add_origin_filters(cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(!cf->পরবর্তী || !cf->পরবর্তী->connected)
    যাও connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_setup_connect(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 bool *done)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  ফলাফল = cf_setup_connect_steps(cf, তথ্য, done);

  যদি(ফলাফল == CURLE_AGAIN) {
    ++ctx->retry_গুনতি;
    যদি(ctx->retry_গুনতি > 5) /* arbitrary limit, better just timeout? */
      ফেরত CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(তথ্য, cf, "retrying connect, %d. time", ctx->retry_গুনতি);
    Curl_conn_cf_discard_chain(&cf->পরবর্তী, তথ্য);
    ctx->state = CF_SETUP_INIT;
    যাও retry;
  }
  ফেরত ফলাফল;
}

স্থির শূন্য cf_setup_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(তথ্য, cf, "destroy");
  curlx_safefree(ctx);
}

কাঠামো Curl_cftype Curl_cft_setup = {
  "SETUP",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_setup_destroy,
  cf_setup_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

স্থির CURLcode cf_setup_create(কাঠামো Curl_cfilter **pcf,
                                কাঠামো Curl_easy *তথ্য,
                                uint8_t transport,
                                পূর্ণসংখ্যা ssl_mode)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_setup_ctx *ctx;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)তথ্য;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  যদি(ফলাফল)
    যাও out;
  ctx = ফাঁকা;

out:
  *pcf = ফলাফল ? ফাঁকা : cf;
  যদি(ctx) {
    curlx_free(ctx);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_cf_setup_add(কাঠামো Curl_easy *তথ্য,
                           কাঠামো connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           পূর্ণসংখ্যা ssl_mode)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(তথ্য);
  ফলাফল = cf_setup_create(&cf, তথ্য, transport, ssl_mode);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_add(তথ্য, conn, sockindex, cf);
out:
  ফেরত ফলাফল;
}

CURLcode Curl_cf_setup_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                    কাঠামো Curl_easy *তথ্য,
                                    uint8_t transport,
                                    পূর্ণসংখ্যা ssl_mode)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  ফলাফল = cf_setup_create(&cf, তথ্য, transport, ssl_mode);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
out:
  ফেরত ফলাফল;
}
