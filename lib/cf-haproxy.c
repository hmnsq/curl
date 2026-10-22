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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-haproxy.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "select.h"


തരനിർവചനം എണ്ണൽ {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

ഘടന cf_haproxy_ctx {
  പൂർണ്ണസംഖ്യ state;
  ഘടന dynbuf data_out;
};

സ്ഥിതം ശൂന്യം cf_haproxy_ctx_free(ഘടന cf_haproxy_ctx *ctx)
{
  എങ്കിൽ(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

സ്ഥിതം CURLcode cf_haproxy_date_out_set(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  സ്ഥിരം അക്ഷരം *client_source_ip;
  സ്ഥിരം അക്ഷരം *client_dest_ip;
  ഘടന cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ഘടന ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  എങ്കിൽ(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  അല്ലെങ്കിൽ {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  എങ്കിൽ(result)
    തിരികെ result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  എങ്കിൽ(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  അല്ലെങ്കിൽ {
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
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_haproxy_connect(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   bool *done)
{
  ഘടന cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  എങ്കിൽ(result || !*done)
    തിരികെ result;

  തിരഞ്ഞെടുപ്പ്(ctx->state) {
  അവസ്ഥ HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  അവസ്ഥ HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    എങ്കിൽ(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      എങ്കിൽ(result) {
        എങ്കിൽ(result != CURLE_AGAIN)
          പോകുക out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      എങ്കിൽ(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        പോകുക out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  സ്ഥിരസ്ഥിതി:
    curlx_dyn_free(&ctx->data_out);
    നിർത്തുക;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_haproxy_destroy(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

സ്ഥിതം CURLcode cf_haproxy_adjust_pollset(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          ഘടന easy_pollset *ps)
{
  എങ്കിൽ(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    തിരികെ Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  തിരികെ CURLE_OK;
}

ഘടന Curl_cftype Curl_cft_haproxy = {
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

സ്ഥിതം CURLcode cf_haproxy_create(ഘടന Curl_cfilter **pcf,
                                  ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_haproxy_ctx *ctx;
  CURLcode result;

  (ശൂന്യം)data;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  ctx = ശൂന്യ;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? ശൂന്യ : cf;
  തിരികെ result;
}

CURLcode Curl_cf_haproxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                      ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_തിരുകുക_after(cf_at, cf);

out:
  തിരികെ result;
}

#endif /* !CURL_DISABLE_PROXY */
