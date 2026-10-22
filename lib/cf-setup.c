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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-haproxy.h"
#अन्तर्भाव "cf-ip-happy.h"
#अन्तर्भाव "cf-setup.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "socks.h"
#अन्तर्भाव "vquic/cf-capsule.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vtls/vtls.h"


प्रकारनिर्देश गणना {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

संरचना cf_setup_ctx {
  cf_setup_state state;
  पूर्णाङ्क ssl_mode;
  uint8_t transport;
  uint8_t retry_गणनम्;
};

#ifndef CURL_DISABLE_PROXY

स्थिर CURLcode cf_setup_add_haproxy(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  सचे(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    सचे(data->set.haproxyprotocol) {
      सचे(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        पटिनिवत्त CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_निवेशय_after(cf, data);
      सचे(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (पूर्णाङ्क)result);
        पटिनिवत्त result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode cf_setup_add_socks(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  सचे(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    संरचना Curl_peer *second_peer;

    सचे(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    अञ्ञथा
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    सचे(!second_peer)
      पटिनिवत्त CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_निवेशय_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    सचे(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (पूर्णाङ्क)result);
      पटिनिवत्त result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  पटिनिवत्त result;
}

#ifndef CURL_DISABLE_HTTP
स्थिर CURLcode cf_setup_add_http_proxy(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  सचे(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    संरचना Curl_peer *peer = cf->conn->http_proxy.peer;
    संरचना Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    सचे(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_निवेशय_after(
        cf, data, cf->conn->http_proxy.peer);
      सचे(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (पूर्णाङ्क)result);
        पटिनिवत्त result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_निवेशय_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    सचे(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (पूर्णाङ्क)result);
      पटिनिवत्त result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  पटिनिवत्त result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
स्थिर संरचना Curl_peer *conn_get_first_origin(संरचना connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  सचे(conn->socks_proxy.peer)
    पटिनिवत्त conn->socks_proxy.peer;
  सचे(conn->http_proxy.peer)
    पटिनिवत्त conn->http_proxy.peer;
#endif
  पटिनिवत्त (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

स्थिर CURLcode cf_setup_add_ip_happy(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  सचे(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    संरचना Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    संरचना Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    संरचना Curl_peer *tunnel_peer = रिक्त;
    uint8_t first_transport = ctx->transport;

    सचे(!first_peer)
      पटिनिवत्त CURLE_FAILED_INIT;

#सचे !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    सचे(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      सचे((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        पटिनिवत्त CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_निवेशय_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    सचे(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (पूर्णाङ्क)result);
      पटिनिवत्त result;
    }

    सचे(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    अञ्ञथा {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  पटिनिवत्त result;
}

स्थिर CURLcode cf_setup_add_origin_filters(संरचना Curl_cfilter *cf,
                                            संरचना Curl_easy *data)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (शून्य)data; /* not used in all builds */
  सचे(ctx->state < CF_SETUP_CNNCT_SSL) {
#सचे !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    सचे(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      संरचना Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      संरचना Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_निवेशय_after(cf, data);
      सचे(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (पूर्णाङ्क)result);
        पटिनिवत्त result;
      }
      result = Curl_cf_quic_निवेशय_after(cf, data, origin, peer);
      सचे(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (पूर्णाङ्क)result);
        पटिनिवत्त result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    अञ्ञथा
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    सचे((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      सचे(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_निवेशय_after(cf, data, cf->conn->origin);
      }
      अञ्ञथा
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_निवेशय_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      सचे(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (पूर्णाङ्क)result);
        पटिनिवत्त result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode cf_setup_connect_steps(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       bool *done)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  सचे(cf->connected) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  सचे(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    सचे(result || !*done)
      पटिनिवत्त result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  सचे(result)
    पटिनिवत्त result;
  सचे(!cf->next || !cf->next->connected)
    गच्छ connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  सचे(result)
    पटिनिवत्त result;
  सचे(!cf->next || !cf->next->connected)
    गच्छ connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  सचे(result)
    पटिनिवत्त result;
  सचे(!cf->next || !cf->next->connected)
    गच्छ connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  सचे(result)
    पटिनिवत्त result;
  सचे(!cf->next || !cf->next->connected)
    गच्छ connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  सचे(result)
    पटिनिवत्त result;
  सचे(!cf->next || !cf->next->connected)
    गच्छ connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode cf_setup_connect(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool *done)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  सचे(result == CURLE_AGAIN) {
    ++ctx->retry_गणनम्;
    सचे(ctx->retry_गणनम् > 5) /* arbitrary limit, better just timeout? */
      पटिनिवत्त CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_गणनम्);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    गच्छ retry;
  }
  पटिनिवत्त result;
}

स्थिर शून्य cf_setup_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

संरचना Curl_cftype Curl_cft_setup = {
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

स्थिर CURLcode cf_setup_create(संरचना Curl_cfilter **pcf,
                                संरचना Curl_easy *data,
                                uint8_t transport,
                                पूर्णाङ्क ssl_mode)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (शून्य)data;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  सचे(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  सचे(result)
    गच्छ out;
  ctx = रिक्त;

out:
  *pcf = result ? रिक्त : cf;
  सचे(ctx) {
    curlx_free(ctx);
  }
  पटिनिवत्त result;
}

CURLcode Curl_cf_setup_add(संरचना Curl_easy *data,
                           संरचना connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           पूर्णाङ्क ssl_mode)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  सचे(result)
    गच्छ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  पटिनिवत्त result;
}

CURLcode Curl_cf_setup_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                    संरचना Curl_easy *data,
                                    uint8_t transport,
                                    पूर्णाङ्क ssl_mode)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  सचे(result)
    गच्छ out;
  Curl_conn_cf_निवेशय_after(cf_at, cf);
out:
  पटिनिवत्त result;
}
