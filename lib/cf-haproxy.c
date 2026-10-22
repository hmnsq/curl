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

#ifndef CURL_DISABLE_PROXY

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-haproxy.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "select.h"


પ્રકારવ્યાખ્યા ગણના {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

સંરચના cf_haproxy_ctx {
  પૂર્ણાંક state;
  સંરચના dynbuf data_out;
};

સ્થિર શૂન્ય cf_haproxy_ctx_free(સંરચના cf_haproxy_ctx *ctx)
{
  જો(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

સ્થિર CURLcode cf_haproxy_date_out_set(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  અચલ અક્ષર *client_source_ip;
  અચલ અક્ષર *client_dest_ip;
  સંરચના cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  સંરચના ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  જો(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  નહીંતર {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  જો(result)
    પાછા result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  જો(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  નહીંતર {
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
  પાછા result;
}

સ્થિર CURLcode cf_haproxy_connect(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   bool *done)
{
  સંરચના cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  જો(result || !*done)
    પાછા result;

  પસંદગી(ctx->state) {
  સ્થિતિ HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    જો(result)
      જાઓ out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  સ્થિતિ HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    જો(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      જો(result) {
        જો(result != CURLE_AGAIN)
          જાઓ out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      જો(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        જાઓ out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  મૂળભૂત:
    curlx_dyn_free(&ctx->data_out);
    તોડો;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  પાછા result;
}

સ્થિર શૂન્ય cf_haproxy_destroy(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

સ્થિર CURLcode cf_haproxy_adjust_pollset(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          સંરચના easy_pollset *ps)
{
  જો(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    પાછા Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  પાછા CURLE_OK;
}

સંરચના Curl_cftype Curl_cft_haproxy = {
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

સ્થિર CURLcode cf_haproxy_create(સંરચના Curl_cfilter **pcf,
                                  સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_haproxy_ctx *ctx;
  CURLcode result;

  (શૂન્ય)data;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  જો(result)
    જાઓ out;
  ctx = ખાલી;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? ખાલી : cf;
  પાછા result;
}

CURLcode Curl_cf_haproxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                      સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  જો(result)
    જાઓ out;
  Curl_conn_cf_ઉમેરો_after(cf_at, cf);

out:
  પાછા result;
}

#endif /* !CURL_DISABLE_PROXY */
