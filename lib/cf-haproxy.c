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

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-haproxy.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "select.h"


प्रकारनिर्देश गणना {
  HAPROXY_INIT,     /* init/default/no tunnel state */
  HAPROXY_SEND,     /* data_out being sent */
  HAPROXY_समाप्तम्      /* all work done */
} haproxy_state;

संरचना cf_haproxy_ctx {
  पूर्णाङ्क state;
  संरचना dynbuf दत्त_बहिर्गमनम्;
};

स्थिर शून्य cf_haproxy_ctx_free(संरचना cf_haproxy_ctx *ctx)
{
  यदि(ctx) {
    curlx_dyn_free(&ctx->दत्त_बहिर्गमनम्);
    curlx_free(ctx);
  }
}

स्थिर CURLcode cf_haproxy_date_बहिर्गमनम्_समुच्चयः(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त)
{
  /* We fake a client connection report to the upstream server
   * with the HAProxy protocol, reporting the client's source
   * and destination IP addresses and ports.
   * addresses: either the ones used to talk to the upstream
   *            OR the value supplied by the user
   * ports: the ports used in the upstream connection */
  ध्रुव वर्ण *client_source_ip;
  ध्रुव वर्ण *client_dest_ip;
  संरचना cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  संरचना ip_quadruple ipquad;
  bool is_ipv6;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->state == HAPROXY_INIT);
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  यदि(Curl_conn_get_first_peer(cf->conn, cf->sockindex)->unix_socket)
    /* the buffer is large enough to hold this! */
    परिणाम = curlx_dyn_addn(&ctx->दत्त_बहिर्गमनम्, STRCONST("PROXY UNKNOWN\r\n"));
  अन्यथा {
#यदि_अन्तः /* USE_UNIX_SOCKETS */
  परिणाम = Curl_conn_cf_get_ip_info(cf->अग्र, दत्त, &is_ipv6, &ipquad);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  client_source_ip = CURL_EASY_STR(दत्त, STRING_HAPROXY_CLIENT_IP);
  यदि(client_source_ip) {
    client_dest_ip = client_source_ip;
    is_ipv6 = !Curl_is_ipv4addr(client_source_ip);
  }
  अन्यथा {
    client_source_ip = ipquad.local_ip;
    client_dest_ip = ipquad.remote_ip;
  }

  परिणाम = curlx_dyn_addf(&ctx->दत्त_बहिर्गमनम्, "PROXY %s %s %s %d %d\r\n",
                          is_ipv6 ? "TCP6" : "TCP4",
                          client_source_ip, client_dest_ip,
                          ipquad.local_पत्तनम्, ipquad.remote_पत्तनम्);

#यदि_परिभाषितम् USE_UNIX_SOCKETS
  }
#यदि_अन्तः /* USE_UNIX_SOCKETS */
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_haproxy_connect(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   bool *समाप्तम्)
{
  संरचना cf_haproxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  size_t len;

  DEBUGASSERT(ctx);
  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, दत्त, समाप्तम्);
  यदि(परिणाम || !*समाप्तम्)
    प्रत्यागम परिणाम;

  विकल्प(ctx->state) {
  पक्ष HAPROXY_INIT:
    परिणाम = cf_haproxy_date_बहिर्गमनम्_समुच्चयः(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    ctx->state = HAPROXY_SEND;
    FALLTHROUGH();
  पक्ष HAPROXY_SEND:
    len = curlx_dyn_len(&ctx->दत्त_बहिर्गमनम्);
    यदि(len > 0) {
      size_t nwritten;
      परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त,
                                 curlx_dyn_uptr(&ctx->दत्त_बहिर्गमनम्), len, FALSE,
                                 &nwritten);
      यदि(परिणाम) {
        यदि(परिणाम != CURLE_AGAIN)
          गच्छ बहिर्गमनम्;
        परिणाम = CURLE_OK;
        nwritten = 0;
      }
      curlx_dyn_पुच्छ(&ctx->दत्त_बहिर्गमनम्, len - nwritten);
      यदि(curlx_dyn_len(&ctx->दत्त_बहिर्गमनम्) > 0) {
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
    }
    ctx->state = HAPROXY_समाप्तम्;
    FALLTHROUGH();
  स्वभाव:
    curlx_dyn_free(&ctx->दत्त_बहिर्गमनम्);
    भङ्ग;
  }

बहिर्गमनम्:
  *समाप्तम् = (!परिणाम) && (ctx->state == HAPROXY_समाप्तम्);
  cf->connected = *समाप्तम्;
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_haproxy_destroy(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त)
{
  CURL_TRC_CF(दत्त, cf, "destroy");
  cf_haproxy_ctx_free(cf->ctx);
}

स्थिर CURLcode cf_haproxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त,
                                          संरचना easy_pollset *ps)
{
  यदि(cf->अग्र->connected && !cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are sending. */
    प्रत्यागम Curl_pollset_समुच्चयः_बहिर्गमनम्_only(
      दत्त, ps, Curl_conn_cf_get_socket(cf, दत्त));
  }
  प्रत्यागम CURLE_OK;
}

संरचना Curl_cftype Curl_cft_haproxy = {
  "HAPROXY",
  CF_TYPE_PROXY | CF_TYPE_सज्जता,
  0,
  cf_haproxy_destroy,
  cf_haproxy_connect,
  Curl_cf_def_shutdown,
  cf_haproxy_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

स्थिर CURLcode cf_haproxy_create(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_haproxy_ctx *ctx;
  CURLcode परिणाम;

  (शून्य)दत्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  ctx->state = HAPROXY_INIT;
  curlx_dyn_init(&ctx->दत्त_बहिर्गमनम्, DYN_HAXPROXY);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_haproxy, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ctx = रिक्त;

बहिर्गमनम्:
  cf_haproxy_ctx_free(ctx);
  *pcf = परिणाम ? रिक्त : cf;
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_haproxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                      संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_haproxy_create(&cf, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_निवेशय_after(cf_at, cf);

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_PROXY */
