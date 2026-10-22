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

#ifndef CURL_DISABLE_PROXY

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-haproxy.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "select.h"


ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

ಸಂರಚನಾ cf_haproxy_ctx {
  ಪೂರ್ಣಾಙ್ಕ state;
  ಸಂರಚನಾ dynbuf data_out;
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_haproxy_ctx_free(ಸಂರಚನಾ cf_haproxy_ctx *ctx)
{
  ಯದಿ(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_haproxy_date_out_set(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ಧ್ರುವ ವರ್ಣ *client_source_ip;
  ಧ್ರುವ ವರ್ಣ *client_dest_ip;
  ಸಂರಚನಾ cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ಸಂರಚನಾ ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  ಯದಿ(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  ಅನ್ಯಥಾ {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  ಯದಿ(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  ಅನ್ಯಥಾ {
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
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_haproxy_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   bool *done)
{
  ಸಂರಚನಾ cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  ಯದಿ(result || !*done)
    ಪ್ರತ್ಯಾಗಮ result;

  ವಿಕಲ್ಪ(ctx->state) {
  ಪಕ್ಷ HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  ಪಕ್ಷ HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    ಯದಿ(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      ಯದಿ(result) {
        ಯದಿ(result != CURLE_AGAIN)
          ಗಚ್ಛ out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      ಯದಿ(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        ಗಚ್ಛ out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  ಸ್ವಭಾವ:
    curlx_dyn_free(&ctx->data_out);
    ಭಙ್ಗ;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_haproxy_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ಸ್ಥಿರ CURLcode cf_haproxy_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ easy_pollset *ps)
{
  ಯದಿ(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    ಪ್ರತ್ಯಾಗಮ Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_haproxy = {
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

ಸ್ಥಿರ CURLcode cf_haproxy_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_haproxy_ctx *ctx;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ctx = ರಿಕ್ತ;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? ರಿಕ್ತ : cf;
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_haproxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_PROXY */
