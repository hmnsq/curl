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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-haproxy.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "select.h"


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_ಮುಗಿದ      /* all work done */
} haproxy_state;

ರಚನೆ cf_haproxy_ctx {
  ಪೂರ್ಣಾಂಕ state;
  ರಚನೆ dynbuf ದತ್ತಾಂಶ_ಹೊರಹೋಗು;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_haproxy_ctx_free(ರಚನೆ cf_haproxy_ctx *ctx)
{
  ಆದರೆ(ctx) {
    curlx_dyn_free(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_haproxy_date_ಹೊರಹೋಗು_ಗಣ(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *client_ಮೂಲ_ip;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *client_dest_ip;
  ರಚನೆ cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  ಆದರೆ(Curl_conn_get_ಮೊದಲ_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು, STRCONST("PROXY UNKNOWN\r\n"));
  ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ_ಅಂತ್ಯ /* USE_UNIX_SOCKETS */
  ಫಲಿತಾಂಶ = Curl_conn_cf_get_ip_info(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &is_ipv6, &ipquad);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  client_ಮೂಲ_ip = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_HAPROXY_CLIENT_IP);
  ಆದರೆ(client_ಮೂಲ_ip) {
    client_dest_ip = client_ಮೂಲ_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_ಮೂಲ_ip);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    client_ಮೂಲ_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  ಫಲಿತಾಂಶ = curlx_dyn_addf(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_ಮೂಲ_ip, client_dest_ip,
                          ipquad.local_ಪೋರ್ಟ್, ipquad.remote_ಪೋರ್ಟ್);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_UNIX_SOCKETS */
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_haproxy_connect(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   bool *ಮುಗಿದ)
{
  ರಚನೆ cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  size_t len;

  DEBUGASSERT(ctx);
  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆಯ್ಕೆ(ctx->state) {
  ಸ್ಥಿತಿ HAPROXY_INIT:
    ಫಲಿತಾಂಶ = cf_haproxy_date_ಹೊರಹೋಗು_ಗಣ(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  ಸ್ಥಿತಿ HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
    ಆದರೆ(len > 0) {
      size_t nwritten;
      ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                                 curlx_dyn_uptr(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು), len, FALSE,
                                 &nwritten);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
          ಹೋಗು ಹೊರಹೋಗು;
        ಫಲಿತಾಂಶ = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_ಬಾಲ(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು, len - nwritten);
      ಆದರೆ(curlx_dyn_len(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು) > 0) {
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ctx->state = HAPROXY_ಮುಗಿದ;
    FALLTHROUGH();
  ಪೂರ್ವನಿಯೋಜಿತ:
    curlx_dyn_free(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
    ನಿಲ್ಲಿಸು;
  }

ಹೊರಹೋಗು:
  *ಮುಗಿದ = (!ಫಲಿತಾಂಶ) && (ctx->state == HAPROXY_ಮುಗಿದ);
  cf->connected = *ಮುಗಿದ;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_haproxy_destroy(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ಸ್ಥಿರ CURLcode cf_haproxy_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ easy_pollset *ps)
{
  ಆದರೆ(cf->ಮುಂದಿನ->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    ಹಿಂತಿರುಗಿ Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(
      ದತ್ತಾಂಶ, ps, Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ));
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_ಸಿದ್ಧತೆ,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ಸ್ಥಿರ CURLcode cf_haproxy_create(ರಚನೆ Curl_cfilter **pcf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_haproxy_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->ದತ್ತಾಂಶ_ಹೊರಹೋಗು, DYN_HAXPROXY);

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = ಖಾಲಿ;

ಹೊರಹೋಗು:
  cf_haproxy_ctx_free(ctx);
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_haproxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_haproxy_create(&cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */
