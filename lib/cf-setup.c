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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "cf-haproxy.h"
#समावेश "cf-ip-happy.h"
#समावेश "cf-setup.h"
#समावेश "curl_trc.h"
#समावेश "connect.h"
#समावेश "http_proxy.h"
#समावेश "socks.h"
#समावेश "vquic/cf-capsule.h"
#समावेश "vquic/vquic.h"
#समावेश "vtls/vtls.h"


प्रकार क्रमागत {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

काष्ठा cf_setup_ctx {
  cf_setup_state state;
  पूर्णांक ssl_mode;
  uint8_t transport;
  uint8_t retry_गिनती;
};

#अगर_अघोषित CURL_DISABLE_PROXY

अटल CURLcode cf_setup_add_haproxy(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    अगर(तथ्य->set.haproxyprotocol) {
      अगर(ctx->transport == TRNSPRT_QUIC) {
        failf(तथ्य, "haproxy protocol does not support QUIC");
        वापस CURLE_UNSUPPORTED_PROTOCOL;
      }
      परिणाम = Curl_cf_haproxy_जोड़ो_after(cf, तथ्य);
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "adding HAPROXY filter failed -> %d",
                    (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      CURL_TRC_CF(तथ्य, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  वापस परिणाम;
}

अटल CURLcode cf_setup_add_socks(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  अगर(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    काष्ठा Curl_peer *second_peer;

    अगर(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    अन्यथा
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    अगर(!second_peer)
      वापस CURLE_FAILED_INIT;

    परिणाम = Curl_cf_socks_proxy_जोड़ो_after(
      cf, तथ्य, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "adding SOCKS filter failed -> %d", (पूर्णांक)परिणाम);
      वापस परिणाम;
    }

    CURL_TRC_CF(तथ्य, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_HTTP
अटल CURLcode cf_setup_add_http_proxy(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    काष्ठा Curl_peer *peer = cf->conn->http_proxy.peer;
    काष्ठा Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#अगर_घोषित USE_SSL
    अगर(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      परिणाम = Curl_cf_ssl_proxy_जोड़ो_after(
        cf, तथ्य, cf->conn->http_proxy.peer);
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      CURL_TRC_CF(तथ्य, cf, "added SSL filter for HTTP proxy");
    }
#पूर्ण_अगर /* USE_SSL */

    परिणाम = Curl_cf_http_proxy_जोड़ो_after(
      cf, तथ्य, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (पूर्णांक)परिणाम);
      वापस परिणाम;
    }
    CURL_TRC_CF(तथ्य, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  वापस परिणाम;
}
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
#पूर्ण_अगर /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
अटल काष्ठा Curl_peer *conn_get_first_origin(काष्ठा connectdata *conn,
                                             int8_t sockindex)
{
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(conn->socks_proxy.peer)
    वापस conn->socks_proxy.peer;
  अगर(conn->http_proxy.peer)
    वापस conn->http_proxy.peer;
#पूर्ण_अगर
  वापस (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

अटल CURLcode cf_setup_add_ip_happy(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    काष्ठा Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    काष्ठा Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    काष्ठा Curl_peer *tunnel_peer = शून्य;
    uint8_t first_transport = ctx->transport;

    अगर(!first_peer)
      वापस CURLE_FAILED_INIT;

#अगर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    अगर(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      अगर((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(तथ्य, "HTTP/3 proxy not possible via SOCKS");
        वापस CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#पूर्ण_अगर /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    परिणाम = Curl_cf_ip_happy_जोड़ो_after(cf, तथ्य, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "adding happy eyeballs failed -> %d", (पूर्णांक)परिणाम);
      वापस परिणाम;
    }

    अगर(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(तथ्य, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    अन्यथा {
      CURL_TRC_CF(तथ्य, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  वापस परिणाम;
}

अटल CURLcode cf_setup_add_origin_filters(काष्ठा Curl_cfilter *cf,
                                            काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)तथ्य; /* not used in all builds */
  अगर(ctx->state < CF_SETUP_CNNCT_SSL) {
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    अगर(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      काष्ठा Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      काष्ठा Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      परिणाम = Curl_cf_capsule_जोड़ो_after(cf, तथ्य);
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "adding capsule filter failed -> %d",
                    (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      परिणाम = Curl_cf_quic_जोड़ो_after(cf, तथ्य, origin, peer);
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "adding QUIC filter failed -> %d", (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      CURL_TRC_CF(तथ्य, cf, "added QUIC filter for origin");
    }
    अन्यथा
#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#अगर_घोषित USE_SSL
    अगर((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#अगर_अघोषित CURL_DISABLE_PROXY
      अगर(cf->conn->bits.origin_is_proxy) {
        परिणाम = Curl_cf_ssl_proxy_जोड़ो_after(cf, तथ्य, cf->conn->origin);
      }
      अन्यथा
#पूर्ण_अगर
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        परिणाम = Curl_cf_ssl_जोड़ो_after(cf, तथ्य,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "adding SSL filter for origin failed -> %d",
                    (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      CURL_TRC_CF(तथ्य, cf, "added SSL filter for origin");
    }
#पूर्ण_अगर /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  वापस परिणाम;
}

अटल CURLcode cf_setup_connect_steps(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       bool *done)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(तथ्य, cf->sockindex, "cf_setup_connect"));

  अगर(cf->अग्र && !cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  परिणाम = cf_setup_add_ip_happy(cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(!cf->अग्र || !cf->अग्र->connected)
    जाओ connect_sub_chain;

#अगर_अघोषित CURL_DISABLE_PROXY
  परिणाम = cf_setup_add_socks(cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(!cf->अग्र || !cf->अग्र->connected)
    जाओ connect_sub_chain;

#अगर_अघोषित CURL_DISABLE_HTTP
  परिणाम = cf_setup_add_http_proxy(cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(!cf->अग्र || !cf->अग्र->connected)
    जाओ connect_sub_chain;
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */

  परिणाम = cf_setup_add_haproxy(cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(!cf->अग्र || !cf->अग्र->connected)
    जाओ connect_sub_chain;
#पूर्ण_अगर /* !CURL_DISABLE_PROXY */

  परिणाम = cf_setup_add_origin_filters(cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(!cf->अग्र || !cf->अग्र->connected)
    जाओ connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  वापस CURLE_OK;
}

अटल CURLcode cf_setup_connect(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 bool *done)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  परिणाम = cf_setup_connect_steps(cf, तथ्य, done);

  अगर(परिणाम == CURLE_AGAIN) {
    ++ctx->retry_गिनती;
    अगर(ctx->retry_गिनती > 5) /* arbitrary limit, better just timeout? */
      वापस CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(तथ्य, cf, "retrying connect, %d. time", ctx->retry_गिनती);
    Curl_conn_cf_discard_chain(&cf->अग्र, तथ्य);
    ctx->state = CF_SETUP_INIT;
    जाओ retry;
  }
  वापस परिणाम;
}

अटल व्योम cf_setup_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(तथ्य, cf, "destroy");
  curlx_safefree(ctx);
}

काष्ठा Curl_cftype Curl_cft_setup = {
  "SETUP",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_setup_destroy,
  cf_setup_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

अटल CURLcode cf_setup_create(काष्ठा Curl_cfilter **pcf,
                                काष्ठा Curl_easy *तथ्य,
                                uint8_t transport,
                                पूर्णांक ssl_mode)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_setup_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)तथ्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  अगर(परिणाम)
    जाओ out;
  ctx = शून्य;

out:
  *pcf = परिणाम ? शून्य : cf;
  अगर(ctx) {
    curlx_free(ctx);
  }
  वापस परिणाम;
}

CURLcode Curl_cf_setup_add(काष्ठा Curl_easy *तथ्य,
                           काष्ठा connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           पूर्णांक ssl_mode)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(तथ्य);
  परिणाम = cf_setup_create(&cf, तथ्य, transport, ssl_mode);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_add(तथ्य, conn, sockindex, cf);
out:
  वापस परिणाम;
}

CURLcode Curl_cf_setup_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                    काष्ठा Curl_easy *तथ्य,
                                    uint8_t transport,
                                    पूर्णांक ssl_mode)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  परिणाम = cf_setup_create(&cf, तथ्य, transport, ssl_mode);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_जोड़ो_after(cf_at, cf);
out:
  वापस परिणाम;
}
