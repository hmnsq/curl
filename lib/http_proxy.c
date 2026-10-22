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
#شامل "curl_setup.h"

#شامل "http_proxy.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#شامل "curl_trc.h"
#شامل "http.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "cf-h1-proxy.h"
#شامل "cf-h2-proxy.h"
#شامل "connect.h"
#شامل "vauth/vauth.h"
#شامل "vquic/vquic.h"
#شامل "curlx/strparse.h"

ایستا CURLcode dynhds_add_custom(ساختار Curl_easy *data,
                                  bool is_connect, صحیح httpversion,
                                  bool is_udp, ساختار dynhds *hds)
{
  ساختار connectdata *conn = data->conn;
  ساختار curl_slist *h[2];
  ساختار curl_slist *headers;
  صحیح numlists = 1; /* by default */
  صحیح i;

  شمارش Curl_proxy_use proxy;

  اگر(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  وگرنه اگر(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  وگرنه
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  انتخاب(proxy) {
  حالت HEADER_SERVER:
    h[0] = data->set.headers;
    بشکن;
  حالت HEADER_PROXY:
    h[0] = data->set.headers;
    اگر(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    بشکن;
  حالت HEADER_CONNECT:
    اگر(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وگرنه
      h[0] = data->set.headers;
    بشکن;
  حالت HEADER_CONNECT_UDP:
    اگر(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وگرنه
      h[0] = data->set.headers;
    بشکن;
  }

  /* loop through one or two lists */
  برای(i = 0; i < numlists; i++) {
    برای(headers = h[i]; headers; headers = headers->next) {
      ساختار Curl_str name;
      ثابت نویسه *مقدار = پوچ;
      size_t valuelen = 0;
      ثابت نویسه *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      اگر(!curlx_str_cspn(&ptr, &name, ";:")) {
        اگر(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          اگر(*ptr) {
            مقدار = ptr;
            valuelen = strlen(مقدار);
          }
          وگرنه {
            /* quirk #1, suppress this header */
            ادامه;
          }
        }
        وگرنه اگر(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          اگر(!*ptr) {
            /* quirk #2, send an empty header */
            مقدار = "";
            valuelen = 0;
          }
          وگرنه {
            /* this may be used for something else in the future,
             * ignore this for now */
            ادامه;
          }
        }
        وگرنه
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          ادامه;
      }
      وگرنه
        /* no name, move on */
        ادامه;

      DEBUGASSERT(curlx_strlen(&name) && مقدار);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      اگر(!curlx_strlen(&name))
        ادامه;
      اگر(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وگرنه اگر(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وگرنه اگر(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وگرنه اگر(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      وگرنه اگر((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      وگرنه اگر((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      وگرنه {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          مقدار, valuelen);
        اگر(result)
          بازگشت result;
      }
    }
  }

  بازگشت CURLE_OK;
}

ساختار cf_proxy_ctx {
  ساختار Curl_peer *peer; /* proxy */
  ساختار Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

ایستا صحیح proxy_http_ver_major(proxy_http_ver ver)
{
  انتخاب(ver) {
  حالت PROXY_HTTP_V1:
    بازگشت 11;
  حالت PROXY_HTTP_V2:
    بازگشت 20;
  حالت PROXY_HTTP_V3:
    بازگشت 30;
  }
  بازگشت 0;
}

ایستا CURLcode http_proxy_create_CONNECT(ساختار httpreq **preq,
                                          ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          ساختار Curl_peer *peer,
                                          ساختار Curl_peer *dest,
                                          proxy_http_ver ver)
{
  نویسه *authority = پوچ;
  ثابت نویسه *ua;
  صحیح httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  ساختار httpreq *req = پوچ;

  (تهی)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  اگر(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              پوچ, 0, authority, strlen(authority),
                              پوچ, 0);
  اگر(result)
    برو out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, پوچ, TRUE);
  اگر(result)
    برو out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    اگر(result)
      برو out;
  }

  اگر(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_سطر(&req->headers,
                                      data->req.hd_proxy_auth);
    اگر(result)
      برو out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  اگر(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    اگر(result)
      برو out;
  }

  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    اگر(result)
      برو out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  اگر(result && req) {
    Curl_http_req_free(req);
    req = پوچ;
  }
  curlx_free(authority);
  *preq = req;
  بازگشت result;
}

ایستا CURLcode http_proxy_create_CONNECTUDP(ساختار httpreq **preq,
                                             ساختار Curl_cfilter *cf,
                                             ساختار Curl_easy *data,
                                             ساختار Curl_peer *peer,
                                             ساختار Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ثابت نویسه *proxy_scheme = "http", *ua;
  صحیح httpversion = proxy_http_ver_major(ver);
  نویسه *authority = پوچ;
  نویسه *path = پوچ;
  نویسه *encoded_host = پوچ;
  ساختار httpreq *req = پوچ;
  CURLcode result;

  اگر(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  اگر(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  اگر(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ثابت نویسه *s = dest->hostname;
    نویسه *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    اگر(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
    d = encoded_host;
    وقتی(*s) {
      اگر(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      وگرنه
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned صحیح)dest->port);
  }
  وگرنه {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned صحیح)dest->port);
  }

  اگر(!path) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  اگر(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    اگر(result)
      برو out;
  }
  وگرنه اگر(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    اگر(result)
      برو out;
  }
  وگرنه {
    result = CURLE_FAILED_INIT;
    برو out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, پوچ, TRUE);
  اگر(result)
    برو out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    اگر(result)
      برو out;
  }

  اگر(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_سطر(&req->headers,
                                      data->req.hd_proxy_auth);
    اگر(result)
      برو out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    اگر(result)
      برو out;
  }

  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    اگر(result)
      برو out;
  }

  اگر(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    اگر(result)
      برو out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    اگر(result)
      برو out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    اگر(result)
      برو out;
  }
  وگرنه {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    اگر(result)
      برو out;

    اگر(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      اگر(result)
        برو out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  اگر(result && req) {
    Curl_http_req_free(req);
    req = پوچ;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  بازگشت result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    ساختار httpreq **preq, ساختار Curl_cfilter *cf,
    ساختار Curl_easy *data, ساختار Curl_peer *peer, ساختار Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  اگر(result)
    بازگشت result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  بازگشت CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
    ساختار http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  ساختار dynhds_entry *capsule_protocol = پوچ;
  ساختار dynhds_entry *auth_reply = پوچ;
  size_t i, header_شمار;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_شمار = Curl_dynhds_شمار(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_شمار);
  برای(i = 0; i < header_شمار; i++) {
    ساختار dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    اگر(entry)
      infof(data, "  %s: %s", entry->name, entry->مقدار);
  }

  اگر(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  اگر(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->مقدار);
    result = Curl_http_درون_داد_auth(data, resp->status == 407,
                                  auth_reply->مقدار);
    اگر(result)
      بازگشت result;
    اگر(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      بازگشت CURLE_OK;
    }
  }

  اگر(udp_tunnel) {
    اگر(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      اگر(capsule_protocol) {
        اگر(!strncmp(capsule_protocol->مقدار, "?1", 2) &&
           !capsule_protocol->مقدار[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          بازگشت CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->مقدار);
        *presult = PROXY_INSPECT_FAILED;
        بازگشت CURLE_COULDNT_CONNECT;
      }
      وگرنه {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        بازگشت CURLE_OK;
      }
    }
    وگرنه {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      بازگشت CURLE_COULDNT_CONNECT;
    }
  }

  اگر(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    بازگشت CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  بازگشت CURLE_COULDNT_CONNECT;
}

ایستا CURLcode http_proxy_cf_connect(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      bool *done)
{
  ساختار cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  اگر(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  *done = FALSE;
  اگر(!ctx->sub_filter_installed) {
    ثابت نویسه *alpn = پوچ;

    /* in case of h3_proxy, cf->next will be NULL initially */
    اگر(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    اگر(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    وگرنه اگر(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      انتخاب(ctx->proxytype) {
      حالت CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        بشکن;
      حالت CURLPROXY_HTTPS2:
        alpn = "h2";
        بشکن;
      حالت CURLPROXY_HTTPS3:
        alpn = "h3";
        بشکن;
      پیشفرض:
        alpn = "http/1.1";
        بشکن;
      }
    }

    اگر(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_درج_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      اگر(result)
        برو out;
    }
    وگرنه اگر(!strcmp(alpn, "http/1.1")) {
      صحیح httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_درج_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      اگر(result)
        برو out;
    }
#ifdef USE_NGHTTP2
    وگرنه اگر(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_درج_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      اگر(result)
        برو out;
    }
#endif /* USE_NGHTTP2 */
#اگر defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    وگرنه اگر(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_درج_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      اگر(result)
        برو out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    وگرنه {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      برو out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    برو connect_sub;
  }
  وگرنه {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  اگر(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  بازگشت result;
}

ایستا CURLcode cf_http_proxy_query(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_proxy_ctx *ctx = cf->ctx;
  انتخاب(query) {
  حالت CF_QUERY_HOST_PORT:
    *pres1 = (صحیح)ctx->tunnel_peer->port;
    *((ثابت نویسه **)pres2) = ctx->tunnel_peer->hostname;
    بازگشت CURLE_OK;
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت نویسه **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = پوچ;
    بازگشت CURLE_OK;
  }
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ایستا تهی cf_https_proxy_ctx_free(ساختار cf_proxy_ctx *ctx)
{
  اگر(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ایستا تهی http_proxy_cf_destroy(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  ساختار cf_proxy_ctx *ctx = cf->ctx;
  اگر(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

ساختار Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SETUP,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_درج_after(ساختار Curl_cfilter *cf_at,
                                         ساختار Curl_easy *data,
                                         ساختار Curl_peer *peer,
                                         ساختار Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  ساختار Curl_cfilter *cf;
  ساختار cf_proxy_ctx *ctx = پوچ;
  CURLcode result;

  (تهی)data;
  اگر(!peer || !tunnel_peer)
    بازگشت CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  اگر(result)
    برو out;
  ctx = پوچ;
  Curl_conn_cf_درج_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  بازگشت result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  انتخاب(proxytype) {
  حالت CURLPROXY_HTTPS3:
    بازگشت TRNSPRT_QUIC;
  پیشفرض:
    بازگشت TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
