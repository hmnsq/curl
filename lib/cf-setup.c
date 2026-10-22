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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-haproxy.h"
#અન્તર્ભાવ "cf-ip-happy.h"
#અન્તર્ભાવ "cf-setup.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "socks.h"
#અન્તર્ભાવ "vquic/cf-capsule.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "vtls/vtls.h"


પ્રકારવ્યાખ્યા ગણના {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

સંરચના cf_setup_ctx {
  cf_setup_state state;
  પૂર્ણાંક ssl_mode;
  uint8_t transport;
  uint8_t retry_ગણતરી;
};

#ifndef CURL_DISABLE_PROXY

સ્થિર CURLcode cf_setup_add_haproxy(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    જો(data->set.haproxyprotocol) {
      જો(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        પાછા CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_ઉમેરો_after(cf, data);
      જો(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (પૂર્ણાંક)result);
        પાછા result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  પાછા result;
}

સ્થિર CURLcode cf_setup_add_socks(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  જો(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    સંરચના Curl_peer *second_peer;

    જો(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    નહીંતર
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    જો(!second_peer)
      પાછા CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_ઉમેરો_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    જો(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (પૂર્ણાંક)result);
      પાછા result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  પાછા result;
}

#ifndef CURL_DISABLE_HTTP
સ્થિર CURLcode cf_setup_add_http_proxy(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    સંરચના Curl_peer *peer = cf->conn->http_proxy.peer;
    સંરચના Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    જો(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_ઉમેરો_after(
        cf, data, cf->conn->http_proxy.peer);
      જો(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (પૂર્ણાંક)result);
        પાછા result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_ઉમેરો_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    જો(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (પૂર્ણાંક)result);
      પાછા result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  પાછા result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
સ્થિર સંરચના Curl_peer *conn_get_first_origin(સંરચના connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  જો(conn->socks_proxy.peer)
    પાછા conn->socks_proxy.peer;
  જો(conn->http_proxy.peer)
    પાછા conn->http_proxy.peer;
#endif
  પાછા (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

સ્થિર CURLcode cf_setup_add_ip_happy(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    સંરચના Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    સંરચના Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    સંરચના Curl_peer *tunnel_peer = ખાલી;
    uint8_t first_transport = ctx->transport;

    જો(!first_peer)
      પાછા CURLE_FAILED_INIT;

#જો !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    જો(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      જો((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        પાછા CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_ઉમેરો_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    જો(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (પૂર્ણાંક)result);
      પાછા result;
    }

    જો(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    નહીંતર {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  પાછા result;
}

સ્થિર CURLcode cf_setup_add_origin_filters(સંરચના Curl_cfilter *cf,
                                            સંરચના Curl_easy *data)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (શૂન્ય)data; /* not used in all builds */
  જો(ctx->state < CF_SETUP_CNNCT_SSL) {
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    જો(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      સંરચના Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      સંરચના Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_ઉમેરો_after(cf, data);
      જો(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (પૂર્ણાંક)result);
        પાછા result;
      }
      result = Curl_cf_quic_ઉમેરો_after(cf, data, origin, peer);
      જો(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (પૂર્ણાંક)result);
        પાછા result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    નહીંતર
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    જો((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      જો(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_ઉમેરો_after(cf, data, cf->conn->origin);
      }
      નહીંતર
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_ઉમેરો_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      જો(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (પૂર્ણાંક)result);
        પાછા result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  પાછા result;
}

સ્થિર CURLcode cf_setup_connect_steps(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       bool *done)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  જો(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  જો(result)
    પાછા result;
  જો(!cf->next || !cf->next->connected)
    જાઓ connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  જો(result)
    પાછા result;
  જો(!cf->next || !cf->next->connected)
    જાઓ connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  જો(result)
    પાછા result;
  જો(!cf->next || !cf->next->connected)
    જાઓ connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  જો(result)
    પાછા result;
  જો(!cf->next || !cf->next->connected)
    જાઓ connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  જો(result)
    પાછા result;
  જો(!cf->next || !cf->next->connected)
    જાઓ connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_setup_connect(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 bool *done)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  જો(result == CURLE_AGAIN) {
    ++ctx->retry_ગણતરી;
    જો(ctx->retry_ગણતરી > 5) /* arbitrary limit, better just timeout? */
      પાછા CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_ગણતરી);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    જાઓ retry;
  }
  પાછા result;
}

સ્થિર શૂન્ય cf_setup_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

સંરચના Curl_cftype Curl_cft_setup = {
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

સ્થિર CURLcode cf_setup_create(સંરચના Curl_cfilter **pcf,
                                સંરચના Curl_easy *data,
                                uint8_t transport,
                                પૂર્ણાંક ssl_mode)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (શૂન્ય)data;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  જો(result)
    જાઓ out;
  ctx = ખાલી;

out:
  *pcf = result ? ખાલી : cf;
  જો(ctx) {
    curlx_free(ctx);
  }
  પાછા result;
}

CURLcode Curl_cf_setup_add(સંરચના Curl_easy *data,
                           સંરચના connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           પૂર્ણાંક ssl_mode)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  જો(result)
    જાઓ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  પાછા result;
}

CURLcode Curl_cf_setup_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                    સંરચના Curl_easy *data,
                                    uint8_t transport,
                                    પૂર્ણાંક ssl_mode)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  જો(result)
    જાઓ out;
  Curl_conn_cf_ઉમેરો_after(cf_at, cf);
out:
  પાછા result;
}
