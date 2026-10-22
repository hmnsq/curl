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
  CF_सज्जता_INIT,
  CF_सज्जता_CNNCT_EYEBALLS,
  CF_सज्जता_CNNCT_SOCKS,
  CF_सज्जता_CNNCT_HTTP_PROXY,
  CF_सज्जता_CNNCT_HAPROXY,
  CF_सज्जता_CNNCT_SSL,
  CF_सज्जता_समाप्तम्
} cf_सज्जता_state;

संरचना cf_सज्जता_ctx {
  cf_सज्जता_state state;
  पूर्णाङ्क ssl_बहुलकम्;
  uint8_t transport;
  uint8_t retry_गणनम्;
};

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY

स्थिर CURLcode cf_सज्जता_सङ्कलय_haproxy(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(ctx->state < CF_सज्जता_CNNCT_HAPROXY) {
    यदि(दत्त->समुच्चयः.haproxyprotocol) {
      यदि(ctx->transport == TRNSPRT_QUIC) {
        failf(दत्त, "haproxy protocol does not support QUIC");
        प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
      }
      परिणाम = Curl_cf_haproxy_निवेशय_after(cf, दत्त);
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "adding HAPROXY filter failed -> %d",
                    (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      CURL_TRC_CF(दत्त, cf, "added HAPROXY filter");
    }
    ctx->state = CF_सज्जता_CNNCT_HAPROXY;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_सज्जता_सङ्कलय_socks(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  यदि(ctx->state < CF_सज्जता_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    संरचना Curl_peer *second_peer;

    यदि(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    अन्यथा
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    यदि(!second_peer)
      प्रत्यागम CURLE_FAILED_INIT;

    परिणाम = Curl_cf_socks_proxy_निवेशय_after(
      cf, दत्त, second_peer, cf->conn->ip_संस्करणम्,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "adding SOCKS filter failed -> %d", (पूर्णाङ्क)परिणाम);
      प्रत्यागम परिणाम;
    }

    CURL_TRC_CF(दत्त, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->पत्तनम्);
    ctx->state = CF_सज्जता_CNNCT_SOCKS;
  }
  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
स्थिर CURLcode cf_सज्जता_सङ्कलय_http_proxy(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(ctx->state < CF_सज्जता_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->द्वयङ्काः.origin_is_proxy) {
    संरचना Curl_peer *peer = cf->conn->http_proxy.peer;
    संरचना Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#यदि_परिभाषितम् USE_SSL
    यदि(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      परिणाम = Curl_cf_ssl_proxy_निवेशय_after(
        cf, दत्त, cf->conn->http_proxy.peer);
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      CURL_TRC_CF(दत्त, cf, "added SSL filter for HTTP proxy");
    }
#यदि_अन्तः /* USE_SSL */

    परिणाम = Curl_cf_http_proxy_निवेशय_after(
      cf, दत्त, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (पूर्णाङ्क)परिणाम);
      प्रत्यागम परिणाम;
    }
    CURL_TRC_CF(दत्त, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_सज्जता_CNNCT_HTTP_PROXY;
  }
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* !CURL_DISABLE_HTTP */
#यदि_अन्तः /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
स्थिर संरचना Curl_peer *conn_get_first_origin(संरचना connectdata *conn,
                                             int8_t sockindex)
{
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(conn->socks_proxy.peer)
    प्रत्यागम conn->socks_proxy.peer;
  यदि(conn->http_proxy.peer)
    प्रत्यागम conn->http_proxy.peer;
#यदि_अन्तः
  प्रत्यागम (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

स्थिर CURLcode cf_सज्जता_सङ्कलय_ip_happy(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(ctx->state < CF_सज्जता_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    संरचना Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    संरचना Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    संरचना Curl_peer *tunnel_peer = रिक्त;
    uint8_t first_transport = ctx->transport;

    यदि(!first_peer)
      प्रत्यागम CURLE_FAILED_INIT;

#यदि !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    यदि(cf->conn->http_proxy.peer && !cf->conn->द्वयङ्काः.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      यदि((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(दत्त, "HTTP/3 proxy not possible via SOCKS");
        प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#यदि_अन्तः /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    परिणाम = Curl_cf_ip_happy_निवेशय_after(cf, दत्त, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "adding happy eyeballs failed -> %d", (पूर्णाङ्क)परिणाम);
      प्रत्यागम परिणाम;
    }

    यदि(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(दत्त, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->पत्तनम्);
      ctx->state = CF_सज्जता_CNNCT_HTTP_PROXY;
    }
    अन्यथा {
      CURL_TRC_CF(दत्त, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->पत्तनम्);
      ctx->state = CF_सज्जता_CNNCT_EYEBALLS;
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_सज्जता_सङ्कलय_origin_filters(संरचना Curl_cfilter *cf,
                                            संरचना Curl_easy *दत्त)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)दत्त; /* not used in all builds */
  यदि(ctx->state < CF_सज्जता_CNNCT_SSL) {
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    यदि(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->द्वयङ्काः.origin_is_proxy) {
      संरचना Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      संरचना Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      परिणाम = Curl_cf_capsule_निवेशय_after(cf, दत्त);
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "adding capsule filter failed -> %d",
                    (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      परिणाम = Curl_cf_quic_निवेशय_after(cf, दत्त, origin, peer);
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "adding QUIC filter failed -> %d", (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      CURL_TRC_CF(दत्त, cf, "added QUIC filter for origin");
    }
    अन्यथा
#यदि_अन्तः /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#यदि_परिभाषितम् USE_SSL
    यदि((ctx->ssl_बहुलकम् == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_बहुलकम् != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
      यदि(cf->conn->द्वयङ्काः.origin_is_proxy) {
        परिणाम = Curl_cf_ssl_proxy_निवेशय_after(cf, दत्त, cf->conn->origin);
      }
      अन्यथा
#यदि_अन्तः
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        परिणाम = Curl_cf_ssl_निवेशय_after(cf, दत्त,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "adding SSL filter for origin failed -> %d",
                    (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      CURL_TRC_CF(दत्त, cf, "added SSL filter for origin");
    }
#यदि_अन्तः /* USE_SSL */
    ctx->state = CF_सज्जता_CNNCT_SSL;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_सज्जता_connect_चरणाः(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       bool *समाप्तम्)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(दत्त, cf->sockindex, "cf_setup_connect"));

  यदि(cf->अग्र && !cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  परिणाम = cf_सज्जता_सङ्कलय_ip_happy(cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(!cf->अग्र || !cf->अग्र->connected)
    गच्छ connect_sub_chain;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  परिणाम = cf_सज्जता_सङ्कलय_socks(cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(!cf->अग्र || !cf->अग्र->connected)
    गच्छ connect_sub_chain;

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  परिणाम = cf_सज्जता_सङ्कलय_http_proxy(cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(!cf->अग्र || !cf->अग्र->connected)
    गच्छ connect_sub_chain;
#यदि_अन्तः /* !CURL_DISABLE_HTTP */

  परिणाम = cf_सज्जता_सङ्कलय_haproxy(cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(!cf->अग्र || !cf->अग्र->connected)
    गच्छ connect_sub_chain;
#यदि_अन्तः /* !CURL_DISABLE_PROXY */

  परिणाम = cf_सज्जता_सङ्कलय_origin_filters(cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(!cf->अग्र || !cf->अग्र->connected)
    गच्छ connect_sub_chain;

  ctx->state = CF_सज्जता_समाप्तम्;
  cf->connected = TRUE;
  *समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_सज्जता_connect(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 bool *समाप्तम्)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  परिणाम = cf_सज्जता_connect_चरणाः(cf, दत्त, समाप्तम्);

  यदि(परिणाम == CURLE_AGAIN) {
    ++ctx->retry_गणनम्;
    यदि(ctx->retry_गणनम् > 5) /* arbitrary limit, better just timeout? */
      प्रत्यागम CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(दत्त, cf, "retrying connect, %d. time", ctx->retry_गणनम्);
    Curl_conn_cf_discard_chain(&cf->अग्र, दत्त);
    ctx->state = CF_सज्जता_INIT;
    गच्छ retry;
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_सज्जता_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_सज्जता_ctx *ctx = cf->ctx;

  CURL_TRC_CF(दत्त, cf, "destroy");
  curlx_safefree(ctx);
}

संरचना Curl_cftype Curl_cft_सज्जता = {
  "SETUP",
  CF_TYPE_सज्जता,
  CURL_LOG_LVL_NONE,
  cf_सज्जता_destroy,
  cf_सज्जता_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

स्थिर CURLcode cf_सज्जता_create(संरचना Curl_cfilter **pcf,
                                संरचना Curl_easy *दत्त,
                                uint8_t transport,
                                पूर्णाङ्क ssl_बहुलकम्)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_सज्जता_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)दत्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  ctx->state = CF_सज्जता_INIT;
  ctx->ssl_बहुलकम् = ssl_बहुलकम्;
  ctx->transport = transport;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_सज्जता, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ctx = रिक्त;

बहिर्गमनम्:
  *pcf = परिणाम ? रिक्त : cf;
  यदि(ctx) {
    curlx_free(ctx);
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_सज्जता_सङ्कलय(संरचना Curl_easy *दत्त,
                           संरचना connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           पूर्णाङ्क ssl_बहुलकम्)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(दत्त);
  परिणाम = cf_सज्जता_create(&cf, दत्त, transport, ssl_बहुलकम्);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_सङ्कलय(दत्त, conn, sockindex, cf);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_सज्जता_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                    संरचना Curl_easy *दत्त,
                                    uint8_t transport,
                                    पूर्णाङ्क ssl_बहुलकम्)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  परिणाम = cf_सज्जता_create(&cf, दत्त, transport, ssl_बहुलकम्);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_निवेशय_after(cf_at, cf);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}
