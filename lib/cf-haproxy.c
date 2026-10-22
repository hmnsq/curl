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

#ifndef CURL_DISABLE_PROXY

#include "urldata.h"
#include "cfilters.h"
#include "cf-haproxy.h"
#include "connect.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "select.h"


typedef ga_nanaa {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

sa_nracanaa cf_haproxy_ctx {
  puur_naa_nka state;
  sa_nracanaa dynbuf data_out;
};

static void cf_haproxy_ctx_free(sa_nracanaa cf_haproxy_ctx *ctx)
{
  ᯀᯮᯝ(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

static CURLcode cf_haproxy_date_out_set(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  const var_na *client_source_ip;
  const var_na *client_dest_ip;
  sa_nracanaa cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  sa_nracanaa ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  ᯀᯮᯝ(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  ᯀᯮᯝᯂᯪ {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  ᯀᯮᯝ(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  ᯀᯮᯝᯂᯪ {
    client_source_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  result = curlx_dyn_addf(&ctx->data_out, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_source_ip, client_dest_ip,
                          ipquad.local_port, ipquad.remote_port);

#ifdef USE_UNIX_SOCKETS
  }
#endif /* USE_UNIX_SOCKETS */
  ᯀᯮᯞ result;
}

static CURLcode cf_haproxy_connect(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   bool *done)
{
  sa_nracanaa cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  ᯀᯮᯝ(cf->connected) {
    *done = TRUE;
    ᯀᯮᯞ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  ᯀᯮᯝ(result || !*done)
    ᯀᯮᯞ result;

  switch(ctx->state) {
  pak_sa HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    ᯀᯮᯝ(result)
      goto out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  pak_sa HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    ᯀᯮᯝ(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      ᯀᯮᯝ(result) {
        ᯀᯮᯝ(result != CURLE_AGAIN)
          goto out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      ᯀᯮᯝ(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        goto out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  default:
    curlx_dyn_free(&ctx->data_out);
    bha_nga;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  ᯀᯮᯞ result;
}

static void cf_haproxy_destroy(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

static CURLcode cf_haproxy_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          sa_nracanaa easy_pollset *ps)
{
  ᯀᯮᯝ(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    ᯀᯮᯞ Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  ᯀᯮᯞ CURLE_OK;
}

sa_nracanaa Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_SETUP,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

static CURLcode cf_haproxy_create(sa_nracanaa Curl_cfilter **pcf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa cf_haproxy_ctx *ctx;
  CURLcode result;

  (void)data;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᯀᯮᯝ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  ᯀᯮᯝ(result)
    goto out;
  ctx = NULL;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  ᯀᯮᯞ result;
}

CURLcode Curl_cf_haproxy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  ᯀᯮᯝ(result)
    goto out;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  ᯀᯮᯞ result;
}

#endif /* !CURL_DISABLE_PROXY */
