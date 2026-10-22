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
#ضمن "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "cf-haproxy.h"
#ضمن "connect.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "select.h"


تعريفنوع تعداد {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

بنية cf_haproxy_ctx {
  صحيح state;
  بنية dynbuf data_out;
};

ساكن فراغ cf_haproxy_ctx_free(بنية cf_haproxy_ctx *ctx)
{
  ܐܢ(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_haproxy_date_out_set(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ثابت حرف *client_source_ip;
  ثابت حرف *client_dest_ip;
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  بنية ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  ܐܢ(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  ܐܚܪܢܐ {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  ܐܢ(result)
    ܦܢܐ result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  ܐܢ(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  ܐܚܪܢܐ {
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
  ܦܢܐ result;
}

ساكن CURLcode cf_haproxy_connect(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   bool *done)
{
  بنية cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  ܐܢ(cf->connected) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  ܐܢ(result || !*done)
    ܦܢܐ result;

  بدل(ctx->state) {
  حالة HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    ܐܢ(result)
      اذهب out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  حالة HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    ܐܢ(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      ܐܢ(result) {
        ܐܢ(result != CURLE_AGAIN)
          اذهب out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      ܐܢ(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        اذهب out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  افتراضي:
    curlx_dyn_free(&ctx->data_out);
    اكسر;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  ܦܢܐ result;
}

ساكن فراغ cf_haproxy_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ساكن CURLcode cf_haproxy_adjust_pollset(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data,
                                          بنية easy_pollset *ps)
{
  ܐܢ(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    ܦܢܐ Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  ܦܢܐ CURLE_OK;
}

بنية Curl_cftype Curl_cft_haproxy = {
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

ساكن CURLcode cf_haproxy_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *data)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_haproxy_ctx *ctx;
  CURLcode result;

  (فراغ)data;
  ctx = curlx_calloc(1, حجم(*ctx));
  ܐܢ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  ܐܢ(result)
    اذهب out;
  ctx = عدم;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? عدم : cf;
  ܦܢܐ result;
}

CURLcode Curl_cf_haproxy_insert_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *data)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  ܐܢ(result)
    اذهب out;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  ܦܢܐ result;
}

#endif /* !CURL_DISABLE_PROXY */
