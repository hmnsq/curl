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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-haproxy.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "select.h"


ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

ସଂରଚନା cf_haproxy_ctx {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା state;
  ସଂରଚନା dynbuf data_out;
};

ସ୍ଥିର ଶୂନ୍ୟ cf_haproxy_ctx_free(ସଂରଚନା cf_haproxy_ctx *ctx)
{
  ଯଦି(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

ସ୍ଥିର CURLcode cf_haproxy_date_out_set(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ଅଚଳ ଅକ୍ଷର *client_source_ip;
  ଅଚଳ ଅକ୍ଷର *client_dest_ip;
  ସଂରଚନା cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ସଂରଚନା ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  ଯଦି(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  ଅନ୍ୟଥା {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  ଯଦି(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  ଅନ୍ୟଥା {
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
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_haproxy_connect(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   bool *done)
{
  ସଂରଚନା cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  ଯଦି(result || !*done)
    ଫେରନ୍ତୁ result;

  ବାଛ(ctx->state) {
  ସ୍ଥିତି HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    ଯଦି(result)
      ଯାଅ out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  ସ୍ଥିତି HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    ଯଦି(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      ଯଦି(result) {
        ଯଦି(result != CURLE_AGAIN)
          ଯାଅ out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      ଯଦି(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        ଯାଅ out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    curlx_dyn_free(&ctx->data_out);
    ଭାଙ୍ଗ;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_haproxy_destroy(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

ସ୍ଥିର CURLcode cf_haproxy_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା easy_pollset *ps)
{
  ଯଦି(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    ଫେରନ୍ତୁ Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା Curl_cftype Curl_cft_haproxy = {
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

ସ୍ଥିର CURLcode cf_haproxy_create(ସଂରଚନା Curl_cfilter **pcf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_haproxy_ctx *ctx;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  ଯଦି(result)
    ଯାଅ out;
  ctx = ଖାଲି;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? ଖାଲି : cf;
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_haproxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  ଯଦି(result)
    ଯାଅ out;
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);

out:
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_PROXY */
