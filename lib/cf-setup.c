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
#include "curl_setup.h"

#include "urldata.h"
#include "cfilters.h"
#include "cf-haproxy.h"
#include "cf-ip-happy.h"
#include "cf-setup.h"
#include "curl_trc.h"
#include "connect.h"
#include "http_proxy.h"
#include "socks.h"
#include "vquic/cf-capsule.h"
#include "vquic/vquic.h"
#include "vtls/vtls.h"


typedef ga_nanaa {
  CF_SETUP_INIT,
  CF_SETUP_CNNCT_EYEBALLS,
  CF_SETUP_CNNCT_SOCKS,
  CF_SETUP_CNNCT_HTTP_PROXY,
  CF_SETUP_CNNCT_HAPROXY,
  CF_SETUP_CNNCT_SSL,
  CF_SETUP_DONE
} cf_setup_state;

sa_nracanaa cf_setup_ctx {
  cf_setup_state state;
  පූර්ණ_සංඛ්‍යා ssl_mode;
  uint8_t transport;
  uint8_t retry_count;
};

#ifndef CURL_DISABLE_PROXY

static CURLcode cf_setup_add_haproxy(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  නම්(ctx->state < CF_SETUP_CNNCT_HAPROXY) {
    නම්(data->set.haproxyprotocol) {
      නම්(ctx->transport == TRNSPRT_QUIC) {
        failf(data, "haproxy protocol does not support QUIC");
        ආපසු CURLE_UNSUPPORTED_PROTOCOL;
      }
      result = Curl_cf_haproxy_insert_after(cf, data);
      නම්(result) {
        CURL_TRC_CF(data, cf, "adding HAPROXY filter failed -> %d",
                    (පූර්ණ_සංඛ්‍යා)result);
        ආපසු result;
      }
      CURL_TRC_CF(data, cf, "added HAPROXY filter");
    }
    ctx->state = CF_SETUP_CNNCT_HAPROXY;
  }
  ආපසු result;
}

static CURLcode cf_setup_add_socks(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  නම්(ctx->state < CF_SETUP_CNNCT_SOCKS && cf->conn->socks_proxy.peer) {
    /* Add a SOCKS proxy to go through `first_peer` to `second_peer`*/
    sa_nracanaa Curl_peer *second_peer;

    නම්(cf->conn->http_proxy.peer)
      second_peer = cf->conn->http_proxy.peer;
    නැතහොත්
      second_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
    නම්(!second_peer)
      ආපසු CURLE_FAILED_INIT;

    result = Curl_cf_socks_proxy_insert_after(
      cf, data, second_peer, cf->conn->ip_version,
      cf->conn->socks_proxy.proxytype,
      cf->conn->socks_proxy.creds);
    නම්(result) {
      CURL_TRC_CF(data, cf, "adding SOCKS filter failed -> %d", (පූර්ණ_සංඛ්‍යා)result);
      ආපසු result;
    }

    CURL_TRC_CF(data, cf, "added SOCKS filter to %s:%u",
                second_peer->hostname, second_peer->port);
    ctx->state = CF_SETUP_CNNCT_SOCKS;
  }
  ආපසු result;
}

#ifndef CURL_DISABLE_HTTP
static CURLcode cf_setup_add_http_proxy(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  නම්(ctx->state < CF_SETUP_CNNCT_HTTP_PROXY &&
     cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
    sa_nracanaa Curl_peer *peer = cf->conn->http_proxy.peer;
    sa_nracanaa Curl_peer *tunnel_peer =
      Curl_conn_get_destination(cf->conn, cf->sockindex);

#ifdef USE_SSL
    නම්(CURL_PROXY_IS_HTTPS(cf->conn->http_proxy.proxytype) &&
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) {
      result = Curl_cf_ssl_proxy_insert_after(
        cf, data, cf->conn->http_proxy.peer);
      නම්(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for HTTP proxy failed -> %d",
                    (පූර්ණ_සංඛ්‍යා)result);
        ආපසු result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for HTTP proxy");
    }
#endif /* USE_SSL */

    result = Curl_cf_http_proxy_insert_after(
      cf, data, peer, tunnel_peer,
      ctx->transport, cf->conn->http_proxy.proxytype);
    නම්(result) {
      CURL_TRC_CF(data, cf, "adding HTTP proxy tunnel filter failed -> %d",
                  (පූර්ණ_සංඛ්‍යා)result);
      ආපසු result;
    }
    CURL_TRC_CF(data, cf, "added HTTP proxy tunnel filter");
    ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
  }
  ආපසු result;
}
#endif /* !CURL_DISABLE_HTTP */
#endif /* CURL_DISABLE_PROXY */

/* Get the origin curl connects its socket to.
 * Can be origin or the first proxy. */
static sa_nracanaa Curl_peer *conn_get_first_origin(sa_nracanaa connectdata *conn,
                                             int8_t sockindex)
{
#ifndef CURL_DISABLE_PROXY
  නම්(conn->socks_proxy.peer)
    ආපසු conn->socks_proxy.peer;
  නම්(conn->http_proxy.peer)
    ආපසු conn->http_proxy.peer;
#endif
  ආපසු (sockindex == SECONDARYSOCKET) ? conn->origin2 : conn->origin;
}

static CURLcode cf_setup_add_ip_happy(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  නම්(ctx->state < CF_SETUP_CNNCT_EYEBALLS) {
    /* What is the first hop we directly connect to and what transport
     * do we use for it? Only on the first hop we can do Happy Eyeballs.
     * first_origin and first_peer differ on --connect-to. */
    sa_nracanaa Curl_peer *first_origin =
      conn_get_first_origin(cf->conn, cf->sockindex);
    sa_nracanaa Curl_peer *first_peer =
      Curl_conn_get_first_peer(cf->conn, cf->sockindex);
    sa_nracanaa Curl_peer *tunnel_peer = NULL;
    uint8_t first_transport = ctx->transport;

    නම්(!first_peer)
      ආපසු CURLE_FAILED_INIT;

#නම් !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)
    නම්(cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      first_transport =
        Curl_http_proxy_transport(cf->conn->http_proxy.proxytype);
      tunnel_peer = Curl_conn_get_destination(cf->conn, cf->sockindex);
      නම්((first_transport == TRNSPRT_QUIC) && cf->conn->socks_proxy.peer) {
        failf(data, "HTTP/3 proxy not possible via SOCKS");
        ආපසු CURLE_UNSUPPORTED_PROTOCOL;
      }
    }
#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

    result = Curl_cf_ip_happy_insert_after(cf, data, first_origin, first_peer,
                                           first_transport,
                                           tunnel_peer, ctx->transport);
    නම්(result) {
      CURL_TRC_CF(data, cf, "adding happy eyeballs failed -> %d", (පූර්ණ_සංඛ්‍යා)result);
      ආපසු result;
    }

    නම්(tunnel_peer && (first_transport == TRNSPRT_QUIC)) {
      CURL_TRC_CF(data, cf, "happy eyeballing to HTTP/3 proxy %s:%u",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_HTTP_PROXY;
    }
    නැතහොත් {
      CURL_TRC_CF(data, cf, "happy eyeballing to %s %s:%u",
                  tunnel_peer ? "proxy" : "origin",
                  first_peer->hostname, first_peer->port);
      ctx->state = CF_SETUP_CNNCT_EYEBALLS;
    }
  }
  ආපසු result;
}

static CURLcode cf_setup_add_origin_filters(sa_nracanaa Curl_cfilter *cf,
                                            sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (void)data; /* not used in all builds */
  නම්(ctx->state < CF_SETUP_CNNCT_SSL) {
#නම් !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3) && \
    !defined(CURL_DISABLE_PROXY)

    /* Wanting QUIC with an HTTP tunneling filter, we now need to add
     * the QUIC filter on top. Without tunneling, this has already
     * happened in the Happy Eyeball filter. */
    නම්(ctx->transport == TRNSPRT_QUIC &&
       cf->conn->http_proxy.peer && !cf->conn->bits.origin_is_proxy) {
      sa_nracanaa Curl_peer *origin = Curl_conn_get_origin(cf->conn, cf->sockindex);
      sa_nracanaa Curl_peer *peer =
        Curl_conn_get_destination(cf->conn, cf->sockindex);

      result = Curl_cf_capsule_insert_after(cf, data);
      නම්(result) {
        CURL_TRC_CF(data, cf, "adding capsule filter failed -> %d",
                    (පූර්ණ_සංඛ්‍යා)result);
        ආපසු result;
      }
      result = Curl_cf_quic_insert_after(cf, data, origin, peer);
      නම්(result) {
        CURL_TRC_CF(data, cf, "adding QUIC filter failed -> %d", (පූර්ණ_සංඛ්‍යා)result);
        ආපසු result;
      }
      CURL_TRC_CF(data, cf, "added QUIC filter for origin");
    }
    නැතහොත්
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 && CURL_DISABLE_PROXY */
#ifdef USE_SSL
    නම්((ctx->ssl_mode == CURL_CF_SSL_ENABLE ||
        (ctx->ssl_mode != CURL_CF_SSL_DISABLE &&
         cf->conn->scheme->flags & PROTOPT_SSL)) && /* we want SSL */
       !Curl_conn_is_ssl(cf->conn, cf->sockindex)) { /* it is missing */

#ifndef CURL_DISABLE_PROXY
      නම්(cf->conn->bits.origin_is_proxy) {
        result = Curl_cf_ssl_proxy_insert_after(cf, data, cf->conn->origin);
      }
      නැතහොත්
#endif
      {
        /* FTP is a bitch. Wherever we really connect to on the DATA
         * (secondary) connection, many servers require TLS sessions reuse
         * to prove they are talking to the same client.
         * For the TLS session lookup to work, we need to instantiate
         * the SSL filter with the same peers as FIRSTSOCKET. See #22225
         * Meaning: cf->sockindex does not matter here. */
        result = Curl_cf_ssl_insert_after(cf, data,
          Curl_conn_get_origin(cf->conn, FIRSTSOCKET),
          Curl_conn_get_destination(cf->conn, FIRSTSOCKET));
      }
      නම්(result) {
        CURL_TRC_CF(data, cf, "adding SSL filter for origin failed -> %d",
                    (පූර්ණ_සංඛ්‍යා)result);
        ආපසු result;
      }
      CURL_TRC_CF(data, cf, "added SSL filter for origin");
    }
#endif /* USE_SSL */
    ctx->state = CF_SETUP_CNNCT_SSL;
  }
  ආපසු result;
}

static CURLcode cf_setup_connect_steps(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       bool *done)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  නම්(cf->connected) {
    *done = TRUE;
    ආපසු CURLE_OK;
  }

  /* connect current sub-chain */
connect_sub_chain:
  VERBOSE(Curl_conn_trc_filters(data, cf->sockindex, "cf_setup_connect"));

  නම්(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    නම්(result || !*done)
      ආපසු result;
  }

  result = cf_setup_add_ip_happy(cf, data);
  නම්(result)
    ආපසු result;
  නම්(!cf->next || !cf->next->connected)
    goto connect_sub_chain;

#ifndef CURL_DISABLE_PROXY
  result = cf_setup_add_socks(cf, data);
  නම්(result)
    ආපසු result;
  නම්(!cf->next || !cf->next->connected)
    goto connect_sub_chain;

#ifndef CURL_DISABLE_HTTP
  result = cf_setup_add_http_proxy(cf, data);
  නම්(result)
    ආපසු result;
  නම්(!cf->next || !cf->next->connected)
    goto connect_sub_chain;
#endif /* !CURL_DISABLE_HTTP */

  result = cf_setup_add_haproxy(cf, data);
  නම්(result)
    ආපසු result;
  නම්(!cf->next || !cf->next->connected)
    goto connect_sub_chain;
#endif /* !CURL_DISABLE_PROXY */

  result = cf_setup_add_origin_filters(cf, data);
  නම්(result)
    ආපසු result;
  නම්(!cf->next || !cf->next->connected)
    goto connect_sub_chain;

  ctx->state = CF_SETUP_DONE;
  cf->connected = TRUE;
  *done = TRUE;
  ආපසු CURLE_OK;
}

static CURLcode cf_setup_connect(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 bool *done)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;
  CURLcode result;

  /* In some situations, a server/proxy may close the connection and
   * we need to connect again (HTTP/1.x proxy auth, for example).
   * We used to close the filters and reuse them for another attempt,
   * however that complicates filter code and it is simpler to tear them
   * all down and start over. */
retry:
  result = cf_setup_connect_steps(cf, data, done);

  නම්(result == CURLE_AGAIN) {
    ++ctx->retry_count;
    නම්(ctx->retry_count > 5) /* arbitrary limit, better just timeout? */
      ආපසු CURLE_COULDNT_CONNECT;

    CURL_TRC_CF(data, cf, "retrying connect, %d. time", ctx->retry_count);
    Curl_conn_cf_discard_chain(&cf->next, data);
    ctx->state = CF_SETUP_INIT;
    goto retry;
  }
  ආපසු result;
}

static void cf_setup_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_setup_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  curlx_safefree(ctx);
}

sa_nracanaa Curl_cftype Curl_cft_setup = {
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

static CURLcode cf_setup_create(sa_nracanaa Curl_cfilter **pcf,
                                sa_nracanaa Curl_easy *data,
                                uint8_t transport,
                                පූර්ණ_සංඛ්‍යා ssl_mode)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa cf_setup_ctx *ctx;
  CURLcode result = CURLE_OK;

  (void)data;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  නම්(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  ctx->state = CF_SETUP_INIT;
  ctx->ssl_mode = ssl_mode;
  ctx->transport = transport;

  result = Curl_cf_create(&cf, &Curl_cft_setup, ctx);
  නම්(result)
    goto out;
  ctx = NULL;

out:
  *pcf = result ? NULL : cf;
  නම්(ctx) {
    curlx_free(ctx);
  }
  ආපසු result;
}

CURLcode Curl_cf_setup_add(sa_nracanaa Curl_easy *data,
                           sa_nracanaa connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           පූර්ණ_සංඛ්‍යා ssl_mode)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  නම්(result)
    goto out;
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ආපසු result;
}

CURLcode Curl_cf_setup_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                    sa_nracanaa Curl_easy *data,
                                    uint8_t transport,
                                    පූර්ණ_සංඛ්‍යා ssl_mode)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(data);
  result = cf_setup_create(&cf, data, transport, ssl_mode);
  නම්(result)
    goto out;
  Curl_conn_cf_insert_after(cf_at, cf);
out:
  ආපසු result;
}
