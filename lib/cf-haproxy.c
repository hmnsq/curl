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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-haproxy.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "select.h"


రకనిర్వచనం గణన {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_పూర్తి      /* all work done */
} haproxy_state;

నిర్మాణం cf_haproxy_ctx {
  పూర్ణాంకం state;
  నిర్మాణం dynbuf దత్తాంశం_బహిర్గతం;
};

స్థిర శూన్య cf_haproxy_ctx_free(నిర్మాణం cf_haproxy_ctx *ctx)
{
  ఒకవేళ(ctx) {
    curlx_dyn_free(&ctx->దత్తాంశం_బహిర్గతం);
    curlx_free(ctx);
  }
}

స్థిర CURLcode cf_haproxy_date_బహిర్గతం_సమితి(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  స్థిరరాశి అక్షరం *client_ఆధారం_ip;
  స్థిరరాశి అక్షరం *client_dest_ip;
  నిర్మాణం cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;
  నిర్మాణం ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#నిర్వచితమైతే USE_UNIX_SOCKETS
  ఒకవేళ(Curl_conn_get_మొదటి_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    ఫలితం = curlx_dyn_addn(&ctx->దత్తాంశం_బహిర్గతం, STRCONST("PROXY UNKNOWN\r\n"));
  లేకపోతే {
#అయితే_ముగింపు /* USE_UNIX_SOCKETS */
  ఫలితం = Curl_conn_cf_get_ip_info(cf->తదుపరి, దత్తాంశం, &is_ipv6, &ipquad);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  client_ఆధారం_ip = CURL_EASY_STR(దత్తాంశం, STRING_HAPROXY_CLIENT_IP);
  ఒకవేళ(client_ఆధారం_ip) {
    client_dest_ip = client_ఆధారం_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_ఆధారం_ip);
  }
  లేకపోతే {
    client_ఆధారం_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  ఫలితం = curlx_dyn_addf(&ctx->దత్తాంశం_బహిర్గతం, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_ఆధారం_ip, client_dest_ip,
                          ipquad.local_పోర్టు, ipquad.remote_పోర్టు);

#నిర్వచితమైతే USE_UNIX_SOCKETS
  }
#అయితే_ముగింపు /* USE_UNIX_SOCKETS */
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_haproxy_connect(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   bool *పూర్తి)
{
  నిర్మాణం cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;
  size_t len;

  DEBUGASSERT(ctx);
  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  ఫలితం = cf->తదుపరి->cft->do_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
  ఒకవేళ(ఫలితం || !*పూర్తి)
    తిరిగి ఫలితం;

  ఎంపిక(ctx->state) {
  స్థితి HAPROXY_INIT:
    ఫలితం = cf_haproxy_date_బహిర్గతం_సమితి(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  స్థితి HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->దత్తాంశం_బహిర్గతం);
    ఒకవేళ(len > 0) {
      size_t nwritten;
      ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                                 curlx_dyn_uptr(&ctx->దత్తాంశం_బహిర్గతం), len, FALSE,
                                 &nwritten);
      ఒకవేళ(ఫలితం) {
        ఒకవేళ(ఫలితం != CURLE_AGAIN)
          వెళ్ళు బహిర్గతం;
        ఫలితం = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_తోక(&ctx->దత్తాంశం_బహిర్గతం, len - nwritten);
      ఒకవేళ(curlx_dyn_len(&ctx->దత్తాంశం_బహిర్గతం) > 0) {
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
    }
    ctx->state = HAPROXY_పూర్తి;
    FALLTHROUGH();
  అప్రమేయ:
    curlx_dyn_free(&ctx->దత్తాంశం_బహిర్గతం);
    ఆపు;
  }

బహిర్గతం:
  *పూర్తి = (!ఫలితం) && (ctx->state == HAPROXY_పూర్తి);
  cf->connected = *పూర్తి;
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_haproxy_destroy(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

స్థిర CURLcode cf_haproxy_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం easy_pollset *ps)
{
  ఒకవేళ(cf->తదుపరి->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    తిరిగి Curl_pollset_సమితి_బహిర్గతం_only(
      దత్తాంశం, ps, Curl_conn_cf_get_socket(cf, దత్తాంశం));
  }
  తిరిగి CURLE_OK;
}

నిర్మాణం Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_అమరిక,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

స్థిర CURLcode cf_haproxy_create(నిర్మాణం Curl_cfilter **pcf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_haproxy_ctx *ctx;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->దత్తాంశం_బహిర్గతం, DYN_HAXPROXY);

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ctx = ఖాళీ;

బహిర్గతం:
  cf_haproxy_ctx_free(ctx);
  *pcf = ఫలితం ? ఖాళీ : cf;
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_haproxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  ఫలితం = cf_haproxy_create(&cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_చొప్పించు_after(cf_at, cf);

బహిర్గతం:
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */
