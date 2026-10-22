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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_PROXY

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "cf-haproxy.h"
#समावेश "connect.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "select.h"


प्रकार क्रमागत {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_DONE      /* all work done */
} haproxy_state;

काष्ठा cf_haproxy_ctx {
  पूर्णांक state;
  काष्ठा dynbuf तथ्य_out;
};

अटल व्योम cf_haproxy_ctx_free(काष्ठा cf_haproxy_ctx *ctx)
{
  अगर(ctx) {
    curlx_dyn_free(&ctx->तथ्य_out);
    curlx_free(ctx);
  }
}

अटल CURLcode cf_haproxy_date_out_set(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  स्थिर अक्षर *client_source_ip;
  स्थिर अक्षर *client_dest_ip;
  काष्ठा cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  काष्ठा ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#अगर_घोषित USE_UNIX_SOCKETS
  अगर(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    परिणाम = curlx_dyn_addn(&ctx->तथ्य_out, STRCONST("PROXY UNKNOWN\r\n"));
  अन्यथा {
#पूर्ण_अगर /* USE_UNIX_SOCKETS */
  परिणाम = Curl_conn_cf_get_ip_info(cf->अग्र, तथ्य, &is_ipv6, &ipquad);
  अगर(परिणाम)
    वापस परिणाम;

  client_source_ip = CURL_EASY_STR(तथ्य, STRING_HAPROXY_CLIENT_IP);
  अगर(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  अन्यथा {
    client_source_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  परिणाम = curlx_dyn_addf(&ctx->तथ्य_out, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_source_ip, client_dest_ip,
                          ipquad.local_port, ipquad.remote_port);

#अगर_घोषित USE_UNIX_SOCKETS
  }
#पूर्ण_अगर /* USE_UNIX_SOCKETS */
  वापस परिणाम;
}

अटल CURLcode cf_haproxy_connect(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   bool *done)
{
  काष्ठा cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  माप_प्रकार len;

  DEBUGASSERT(ctx);
  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, तथ्य, done);
  अगर(परिणाम || !*done)
    वापस परिणाम;

  चयन(ctx->state) {
  हाल HAPROXY_INIT:
    परिणाम = cf_haproxy_date_out_set(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  हाल HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->तथ्य_out);
    अगर(len > 0) {
      माप_प्रकार nwritten;
      परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य,
                                 curlx_dyn_uptr(&ctx->तथ्य_out), len, FALSE,
                                 &nwritten);
      अगर(परिणाम) {
        अगर(परिणाम != CURLE_AGAIN)
          जाओ out;
        परिणाम = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_पुच्छ(&ctx->तथ्य_out, len - nwritten);
      अगर(curlx_dyn_len(&ctx->तथ्य_out) > 0) {
        परिणाम = CURLE_OK;
        जाओ out;
      }
    }
    ctx->state = HAPROXY_DONE;
    FALLTHROUGH();
  शेष:
    curlx_dyn_free(&ctx->तथ्य_out);
    अवरोध;
  }

out:
  *done = (!परिणाम) && (ctx->state == HAPROXY_DONE);
  cf->connected = *done;
  वापस परिणाम;
}

अटल व्योम cf_haproxy_destroy(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य)
{
  CURL_TRC_CF(तथ्य, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

अटल CURLcode cf_haproxy_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          काष्ठा easy_pollset *ps)
{
  अगर(cf->अग्र->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    वापस Curl_pollset_set_out_only(
      तथ्य, ps, Curl_conn_cf_get_socket(cf, तथ्य));
  }
  वापस CURLE_OK;
}

काष्ठा Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_SETUP,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

अटल CURLcode cf_haproxy_create(काष्ठा Curl_cfilter **pcf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_haproxy_ctx *ctx;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->तथ्य_out, DYN_HAXPROXY);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  अगर(परिणाम)
    जाओ out;
  ctx = शून्य;

out:
  cf_haproxy_ctx_free(ctx);
  *pcf = परिणाम ? शून्य : cf;
  वापस परिणाम;
}

CURLcode Curl_cf_haproxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_haproxy_create(&cf, तथ्य);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_जोड़ो_after(cf_at, cf);

out:
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_PROXY */
