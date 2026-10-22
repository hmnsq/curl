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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-haproxy.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "select.h"


வகைவரை எண்ணிக்கை {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

கட்டமைப்பு cf_haproxy_ctx {
  முழு state;
  கட்டமைப்பு dynbuf data_out;
};

நிலையான வெறுமை cf_haproxy_ctx_free(கட்டமைப்பு cf_haproxy_ctx *ctx)
{
  எனில்(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

நிலையான CURLcode cf_haproxy_date_out_set(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  மாறிலி எழுத்து *client_source_ip;
  மாறிலி எழுத்து *client_dest_ip;
  கட்டமைப்பு cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  கட்டமைப்பு ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  எனில்(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  இல்லையெனில் {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  எனில்(result)
    திரும்பு result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  எனில்(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  இல்லையெனில் {
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
  திரும்பு result;
}

நிலையான CURLcode cf_haproxy_connect(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   bool *done)
{
  கட்டமைப்பு cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  எனில்(result || !*done)
    திரும்பு result;

  தேர்வு(ctx->state) {
  நிலை HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    எனில்(result)
      செல் out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  நிலை HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    எனில்(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      எனில்(result) {
        எனில்(result != CURLE_AGAIN)
          செல் out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      எனில்(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        செல் out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  இயல்பு:
    curlx_dyn_free(&ctx->data_out);
    நிறுத்து;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  திரும்பு result;
}

நிலையான வெறுமை cf_haproxy_destroy(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

நிலையான CURLcode cf_haproxy_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு easy_pollset *ps)
{
  எனில்(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    திரும்பு Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  திரும்பு CURLE_OK;
}

கட்டமைப்பு Curl_cftype Curl_cft_haproxy = {
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

நிலையான CURLcode cf_haproxy_create(கட்டமைப்பு Curl_cfilter **pcf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_haproxy_ctx *ctx;
  CURLcode result;

  (வெறுமை)data;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  எனில்(result)
    செல் out;
  ctx = வெற்று;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? வெற்று : cf;
  திரும்பு result;
}

CURLcode Curl_cf_haproxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  எனில்(result)
    செல் out;
  Curl_conn_cf_செருகு_after(cf_at, cf);

out:
  திரும்பு result;
}

#endif /* !CURL_DISABLE_PROXY */
