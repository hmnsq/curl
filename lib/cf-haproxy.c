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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "cf-haproxy.h"
#شامل "connect.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "select.h"


تعریفنوع شمارش {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

ساختار cf_haproxy_ctx {
  صحیح state;
  ساختار dynbuf data_out;
};

ایستا تهی cf_haproxy_ctx_free(ساختار cf_haproxy_ctx *ctx)
{
  اگر(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

ایستا CURLcode cf_haproxy_date_out_set(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ثابت نویسه *client_source_ip;
  ثابت نویسه *client_dest_ip;
  ساختار cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ساختار ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  اگر(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  وگرنه {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  اگر(result)
    بازگشت result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  اگر(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  وگرنه {
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
  بازگشت result;
}

ایستا CURLcode cf_haproxy_connect(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   bool *done)
{
  ساختار cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  اگر(result || !*done)
    بازگشت result;

  انتخاب(ctx->state) {
  حالت HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    اگر(result)
      برو out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  حالت HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    اگر(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      اگر(result) {
        اگر(result != CURLE_AGAIN)
          برو out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      اگر(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        برو out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  پیشفرض:
    curlx_dyn_free(&ctx->data_out);
    بشکن;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  بازگشت result;
}

ایستا تهی cf_haproxy_destroy(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ایستا CURLcode cf_haproxy_adjust_pollset(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          ساختار easy_pollset *ps)
{
  اگر(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    بازگشت Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  بازگشت CURLE_OK;
}

ساختار Curl_cftype Curl_cft_haproxy = {
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

ایستا CURLcode cf_haproxy_create(ساختار Curl_cfilter **pcf,
                                  ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_haproxy_ctx *ctx;
  CURLcode result;

  (تهی)data;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  اگر(result)
    برو out;
  ctx = پوچ;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? پوچ : cf;
  بازگشت result;
}

CURLcode Curl_cf_haproxy_درج_after(ساختار Curl_cfilter *cf_at,
                                      ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  اگر(result)
    برو out;
  Curl_conn_cf_درج_after(cf_at, cf);

out:
  بازگشت result;
}

#endif /* !CURL_DISABLE_PROXY */
