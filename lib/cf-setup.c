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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-haproxy.h"
#ଅନ୍ତର୍ଭାଵ "cf-ip-happy.h"
#ଅନ୍ତର୍ଭାଵ "cf-setup.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "socks.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-capsule.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"


ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

ସଂରଚନା cf_setup_ctx {
  cf_setup_state state;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_mode;
  uint8_t transport;
  uint8_t retry_ଗଣନା;
};

#ifndef CURL_DISABLE_PROXY

ସ୍ଥିର CURLcode cf_setup_add_haproxy(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    ଯଦି(data->set.haproxyprotocol) {
      ଯଦି(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_ଭର୍ତ୍ତି_after(cf, data);
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_setup_add_socks(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ଯଦି(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    ସଂରଚନା Curl_peer *second_peer;

    ଯଦି(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    ଅନ୍ୟଥା
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    ଯଦି(!second_peer)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_ଭର୍ତ୍ତି_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    ଯଦି(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଫେରନ୍ତୁ result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_HTTP
ସ୍ଥିର CURLcode cf_setup_add_http_proxy(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    ସଂରଚନା Curl_peer *peer = cf->conn->http_proxy.peer;
    ସଂରଚନା Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    ଯଦି(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_ଭର୍ତ୍ତି_after(
        cf, data, cf->conn->http_proxy.peer);
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_ଭର୍ତ୍ତି_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    ଯଦି(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଫେରନ୍ତୁ result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  ଫେରନ୍ତୁ result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
ସ୍ଥିର ସଂରଚନା Curl_peer *conn_get_first_origin(ସଂରଚନା connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  ଯଦି(conn->socks_proxy.peer)
    ଫେରନ୍ତୁ conn->socks_proxy.peer;
  ଯଦି(conn->http_proxy.peer)
    ଫେରନ୍ତୁ conn->http_proxy.peer;
#endif
  ଫେରନ୍ତୁ (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

ସ୍ଥିର CURLcode cf_setup_add_ip_happy(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    ସଂରଚନା Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    ସଂରଚନା Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    ସଂରଚନା Curl_peer *tunnel_peer = ଖାଲି;
    uint8_t first_transport = ctx->transport;

    ଯଦି(!first_peer)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;

#ଯଦି !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    ଯଦି(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      ଯଦି((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_ଭର୍ତ୍ତି_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    ଯଦି(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଫେରନ୍ତୁ result;
    }

    ଯଦି(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    ଅନ୍ୟଥା {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_setup_add_origin_filters(ସଂରଚନା Curl_cfilter *cf,
                                            ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)data; /* not used in all builds */
  ଯଦି(ctx->state < CF_SETUP_CNNCT_SSL) {
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    ଯଦି(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      ସଂରଚନା Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      ସଂରଚନା Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_ଭର୍ତ୍ତି_after(cf, data);
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      result = Curl_cf_quic_ଭର୍ତ୍ତି_after(cf, data, origin, peer);
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    ଅନ୍ୟଥା
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    ଯଦି((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      ଯଦି(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_ଭର୍ତ୍ତି_after(cf, data, cf->conn->origin);
      }
      ଅନ୍ୟଥା
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_ଭର୍ତ୍ତି_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_setup_connect_steps(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       bool *done)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  ଯଦି(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(!cf->next || !cf->next->connected)
    ଯାଅ connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(!cf->next || !cf->next->connected)
    ଯାଅ connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(!cf->next || !cf->next->connected)
    ଯାଅ connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(!cf->next || !cf->next->connected)
    ଯାଅ connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ଯଦି(!cf->next || !cf->next->connected)
    ଯାଅ connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_setup_connect(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 bool *done)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  ଯଦି(result == CURLE_AGAIN) {
    ++ctx->retry_ଗଣନା;
    ଯଦି(ctx->retry_ଗଣନା > 5) /* arbitrary limit, better just timeout? */
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_ଗଣନା);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    ଯାଅ retry;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_setup_destroy(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

ସଂରଚନା Curl_cftype Curl_cft_setup = {
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

ସ୍ଥିର CURLcode cf_setup_create(ସଂରଚନା Curl_cfilter **pcf,
                                ସଂରଚନା Curl_easy *data,
                                uint8_t transport,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_mode)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)data;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  ଯଦି(result)
    ଯାଅ out;
  ctx = ଖାଲି;

out:
  *pcf = result ? ଖାଲି : cf;
  ଯଦି(ctx) {
    curlx_free(ctx);
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_setup_add(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_mode)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  ଯଦି(result)
    ଯାଅ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_setup_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                    ସଂରଚନା Curl_easy *data,
                                    uint8_t transport,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_mode)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  ଯଦି(result)
    ଯାଅ out;
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);
out:
  ଫେରନ୍ତୁ result;
}
