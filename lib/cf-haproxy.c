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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-haproxy.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "select.h"


প্রকারনির্দেশ গণনা {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

সংরচনা cf_haproxy_ctx {
  পূর্ণাঙ্ক state;
  সংরচনা dynbuf data_out;
};

স্থির শূন্য cf_haproxy_ctx_free(সংরচনা cf_haproxy_ctx *ctx)
{
  যদি(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

স্থির CURLcode cf_haproxy_date_out_set(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ধ্রুব বর্ণ *client_source_ip;
  ধ্রুব বর্ণ *client_dest_ip;
  সংরচনা cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  সংরচনা ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  যদি(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  অন্যথা {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  যদি(result)
    প্রত্যাগম result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  যদি(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  অন্যথা {
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
  প্রত্যাগম result;
}

স্থির CURLcode cf_haproxy_connect(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   bool *done)
{
  সংরচনা cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t len;

  DEBUGASSERT(ctx);
  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  যদি(result || !*done)
    প্রত্যাগম result;

  বিকল্প(ctx->state) {
  পক্ষ HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    যদি(result)
      গচ্ছ out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  পক্ষ HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    যদি(len > 0) {
      size_t nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      যদি(result) {
        যদি(result != CURLE_AGAIN)
          গচ্ছ out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      যদি(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        গচ্ছ out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  স্বভাব:
    curlx_dyn_free(&ctx->data_out);
    ভঙ্গ;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  প্রত্যাগম result;
}

স্থির শূন্য cf_haproxy_destroy(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

স্থির CURLcode cf_haproxy_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          সংরচনা easy_pollset *ps)
{
  যদি(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    প্রত্যাগম Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  প্রত্যাগম CURLE_OK;
}

সংরচনা Curl_cftype Curl_cft_haproxy = {
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

স্থির CURLcode cf_haproxy_create(সংরচনা Curl_cfilter **pcf,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_haproxy_ctx *ctx;
  CURLcode result;

  (শূন্য)data;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  যদি(result)
    গচ্ছ out;
  ctx = রিক্ত;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? রিক্ত : cf;
  প্রত্যাগম result;
}

CURLcode Curl_cf_haproxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                      সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  যদি(result)
    গচ্ছ out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  প্রত্যাগম result;
}

#endif /* !CURL_DISABLE_PROXY */
