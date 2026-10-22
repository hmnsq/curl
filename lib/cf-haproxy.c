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


প্রকারলেখ গণনা {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

কাঠামো cf_haproxy_ctx {
  পূর্ণসংখ্যা state;
  কাঠামো dynbuf তথ্য_out;
};

স্থির শূন্য cf_haproxy_ctx_free(কাঠামো cf_haproxy_ctx *ctx)
{
  যদি(ctx) {
    curlx_dyn_free(&ctx->তথ্য_out);
    curlx_free(ctx);
  }
}

স্থির CURLcode cf_haproxy_date_out_set(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ধ্রুবক অক্ষর *client_source_ip;
  ধ্রুবক অক্ষর *client_dest_ip;
  কাঠামো cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;
  কাঠামো ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  যদি(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    ফলাফল = curlx_dyn_addn(&ctx->তথ্য_out, STRCONST("PROXY UNKNOWN\r\n"));
  অন্যথা {
#endif /* USE_UNIX_SOCKETS */
  ফলাফল = Curl_conn_cf_get_ip_info(cf->পরবর্তী, তথ্য, &is_ipv6, &ipquad);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  client_source_ip = CURL_EASY_STR(তথ্য, STRING_HAPROXY_CLIENT_IP);
  যদি(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  অন্যথা {
    client_source_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  ফলাফল = curlx_dyn_addf(&ctx->তথ্য_out, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_source_ip, client_dest_ip,
                          ipquad.local_port, ipquad.remote_port);

#ifdef USE_UNIX_SOCKETS
  }
#endif /* USE_UNIX_SOCKETS */
  ফেরত ফলাফল;
}

স্থির CURLcode cf_haproxy_connect(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   bool *done)
{
  কাঠামো cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;
  size_t len;

  DEBUGASSERT(ctx);
  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  ফলাফল = cf->পরবর্তী->cft->do_connect(cf->পরবর্তী, তথ্য, done);
  যদি(ফলাফল || !*done)
    ফেরত ফলাফল;

  নির্বাচন(ctx->state) {
  ক্ষেত্র HAPROXY_INIT:
    ফলাফল = cf_haproxy_date_out_set(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  ক্ষেত্র HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->তথ্য_out);
    যদি(len > 0) {
      size_t nwritten;
      ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য,
                                 curlx_dyn_uptr(&ctx->তথ্য_out), len, FALSE,
                                 &nwritten);
      যদি(ফলাফল) {
        যদি(ফলাফল != CURLE_AGAIN)
          যাও out;
        ফলাফল = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_লেজ(&ctx->তথ্য_out, len - nwritten);
      যদি(curlx_dyn_len(&ctx->তথ্য_out) > 0) {
        ফলাফল = CURLE_OK;
        যাও out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  পূর্বনির্ধারিত:
    curlx_dyn_free(&ctx->তথ্য_out);
    বিরতি;
  }

out:
  *done = (!ফলাফল) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  ফেরত ফলাফল;
}

স্থির শূন্য cf_haproxy_destroy(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য)
{
  CURL_TRC_CF(তথ্য, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

স্থির CURLcode cf_haproxy_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য,
                                          কাঠামো easy_pollset *ps)
{
  যদি(cf->পরবর্তী->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    ফেরত Curl_pollset_set_out_only(
      তথ্য, ps, Curl_conn_cf_get_socket(cf, তথ্য));
  }
  ফেরত CURLE_OK;
}

কাঠামো Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_SETUP,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

স্থির CURLcode cf_haproxy_create(কাঠামো Curl_cfilter **pcf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_haproxy_ctx *ctx;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->তথ্য_out, DYN_HAXPROXY);

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  যদি(ফলাফল)
    যাও out;
  ctx = ফাঁকা;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = ফলাফল ? ফাঁকা : cf;
  ফেরত ফলাফল;
}

CURLcode Curl_cf_haproxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  ফলাফল = cf_haproxy_create(&cf, তথ্য);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_PROXY */
