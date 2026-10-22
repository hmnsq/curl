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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-haproxy.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "select.h"


प्रकारव्याख्या गणना {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

संरचना cf_haproxy_ctx {
  पूर्णांक state;
  संरचना dynbuf data_out;
};

स्थिर शून्य cf_haproxy_ctx_free(संरचना cf_haproxy_ctx *ctx)
{
  जर(ctx) {
    curlx_dyn_free(&ctx->data_out);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_haproxy_date_out_set(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  स्थिरांक अक्षर *client_source_ip;
  स्थिरांक अक्षर *client_dest_ip;
  संरचना cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#ifdef USE_UNIX_SOCKETS
  जर(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    result = curlx_dyn_addn(&ctx->data_out, STRCONST("PROXY UNKNOWN\r\n"));
  अन्यथा {
#endif /* USE_UNIX_SOCKETS */
  result = Curl_conn_cf_get_ip_info(cf->next, data, &is_ipv6, &ipquad);
  जर(result)
    परत result;

  client_source_ip = CURL_EASY_STR(data, STRING_HAPROXY_CLIENT_IP);
  जर(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  अन्यथा {
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
  परत result;
}

स्थिर CURLcode cf_haproxy_connect(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   bool *done)
{
  संरचना cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode result;
  माप_प्रकार len;

  DEBUGASSERT(ctx);
  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  result = cf->next->cft->do_connect(cf->next, data, done);
  जर(result || !*done)
    परत result;

  निवड(ctx->state) {
  स्थिती HAPROXY_INIT:
    result = cf_haproxy_date_out_set(cf, data);
    जर(result)
      जा out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  स्थिती HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->data_out);
    जर(len > 0) {
      माप_प्रकार nwritten;
      result = Curl_conn_cf_send(cf->next, data,
                                 curlx_dyn_uptr(&ctx->data_out), len, FALSE,
                                 &nwritten);
      जर(result) {
        जर(result != CURLE_AGAIN)
          जा out;
        result = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_tail(&ctx->data_out, len - nwritten);
      जर(curlx_dyn_len(&ctx->data_out) > 0) {
        result = CURLE_OK;
        जा out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  पूर्वनिर्धारित:
    curlx_dyn_free(&ctx->data_out);
    थांबा;
  }

out:
  *done = (!result) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  परत result;
}

स्थिर शून्य cf_haproxy_destroy(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data)
{
  CURL_TRC_CF(data, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

स्थिर CURLcode cf_haproxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          संरचना easy_pollset *ps)
{
  जर(cf->next->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    परत Curl_pollset_set_out_only(
      data, ps, Curl_conn_cf_get_socket(cf, data));
  }
  परत CURLE_OK;
}

संरचना Curl_cftype Curl_cft_haproxy = {
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

स्थिर CURLcode cf_haproxy_create(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_haproxy_ctx *ctx;
  CURLcode result;

  (शून्य)data;
  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->data_out, DYN_HAXPROXY);

  result = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  जर(result)
    जा out;
  ctx = रिक्त;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = result ? रिक्त : cf;
  परत result;
}

CURLcode Curl_cf_haproxy_घाला_after(संरचना Curl_cfilter *cf_at,
                                      संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_haproxy_create(&cf, data);
  जर(result)
    जा out;
  Curl_conn_cf_घाला_after(cf_at, cf);

out:
  परत result;
}

#endif /* !CURL_DISABLE_PROXY */
