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
#ضمن "curl_setup.h"

#ضمن "http_proxy.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#ضمن "curl_trc.h"
#ضمن "http.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "cf-h1-proxy.h"
#ضمن "cf-h2-proxy.h"
#ضمن "connect.h"
#ضمن "vauth/vauth.h"
#ضمن "vquic/vquic.h"
#ضمن "curlx/strparse.h"

ساكن CURLcode dynhds_add_custom(بنية Curl_easy *data,
                                  bool is_connect, صحيح httpversion,
                                  bool is_udp, بنية dynhds *hds)
{
  بنية connectdata *conn = data->conn;
  بنية curl_slist *h[2];
  بنية curl_slist *headers;
  صحيح numlists = 1; /* by default */
  صحيح i;

  تعداد Curl_proxy_use proxy;

  إذا(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  وإلا إذا(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  وإلا
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  بدل(proxy) {
  حالة HEADER_SERVER:
    h[0] = data->set.headers;
    اكسر;
  حالة HEADER_PROXY:
    h[0] = data->set.headers;
    إذا(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة HEADER_CONNECT:
    إذا(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وإلا
      h[0] = data->set.headers;
    اكسر;
  حالة HEADER_CONNECT_UDP:
    إذا(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    وإلا
      h[0] = data->set.headers;
    اكسر;
  }

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(headers = h[i]; headers; headers = headers->next) {
      بنية Curl_str name;
      ثابت حرف *قدر = عدم;
      size_t valuelen = 0;
      ثابت حرف *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      إذا(!curlx_str_cspn(&ptr, &name, ";:")) {
        إذا(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          إذا(*ptr) {
            قدر = ptr;
            valuelen = strlen(قدر);
          }
          وإلا {
            /* quirk #1, suppress this header */
            استمر;
          }
        }
        وإلا إذا(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          إذا(!*ptr) {
            /* quirk #2, send an empty header */
            قدر = "";
            valuelen = 0;
          }
          وإلا {
            /* this may be used for something else in the future,
             * ignore this for now */
            استمر;
          }
        }
        وإلا
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          استمر;
      }
      وإلا
        /* no name, move on */
        استمر;

      DEBUGASSERT(curlx_strlen(&name) && قدر);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      إذا(!curlx_strlen(&name))
        استمر;
      إذا(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      وإلا إذا((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      وإلا إذا((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      وإلا {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          قدر, valuelen);
        إذا(result)
          أرجع result;
      }
    }
  }

  أرجع CURLE_OK;
}

بنية cf_proxy_ctx {
  بنية Curl_peer *peer; /* proxy */
  بنية Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

ساكن صحيح proxy_http_ver_major(proxy_http_ver ver)
{
  بدل(ver) {
  حالة PROXY_HTTP_V1:
    أرجع 11;
  حالة PROXY_HTTP_V2:
    أرجع 20;
  حالة PROXY_HTTP_V3:
    أرجع 30;
  }
  أرجع 0;
}

ساكن CURLcode http_proxy_create_CONNECT(بنية httpreq **preq,
                                          بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data,
                                          بنية Curl_peer *peer,
                                          بنية Curl_peer *dest,
                                          proxy_http_ver ver)
{
  حرف *authority = عدم;
  ثابت حرف *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  بنية httpreq *req = عدم;

  (فراغ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  إذا(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              عدم, 0, authority, strlen(authority),
                              عدم, 0);
  إذا(result)
    اذهب out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(result)
    اذهب out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    إذا(result)
      اذهب out;
  }

  إذا(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_سطر(&req->headers,
                                      data->req.hd_proxy_auth);
    إذا(result)
      اذهب out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  إذا(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    إذا(result)
      اذهب out;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    إذا(result)
      اذهب out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  إذا(result && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  *preq = req;
  أرجع result;
}

ساكن CURLcode http_proxy_create_CONNECTUDP(بنية httpreq **preq,
                                             بنية Curl_cfilter *cf,
                                             بنية Curl_easy *data,
                                             بنية Curl_peer *peer,
                                             بنية Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ثابت حرف *proxy_scheme = "http", *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  حرف *authority = عدم;
  حرف *path = عدم;
  حرف *encoded_host = عدم;
  بنية httpreq *req = عدم;
  CURLcode result;

  إذا(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  إذا(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  إذا(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ثابت حرف *s = dest->hostname;
    حرف *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    إذا(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
    d = encoded_host;
    طالما(*s) {
      إذا(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      وإلا
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned صحيح)dest->port);
  }
  وإلا {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned صحيح)dest->port);
  }

  إذا(!path) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  إذا(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    إذا(result)
      اذهب out;
  }
  وإلا إذا(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    إذا(result)
      اذهب out;
  }
  وإلا {
    result = CURLE_FAILED_INIT;
    اذهب out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(result)
    اذهب out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    إذا(result)
      اذهب out;
  }

  إذا(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_سطر(&req->headers,
                                      data->req.hd_proxy_auth);
    إذا(result)
      اذهب out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    إذا(result)
      اذهب out;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    إذا(result)
      اذهب out;
  }

  إذا(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    إذا(result)
      اذهب out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    إذا(result)
      اذهب out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    إذا(result)
      اذهب out;
  }
  وإلا {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    إذا(result)
      اذهب out;

    إذا(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      إذا(result)
        اذهب out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  إذا(result && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  أرجع result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    بنية httpreq **preq, بنية Curl_cfilter *cf,
    بنية Curl_easy *data, بنية Curl_peer *peer, بنية Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  إذا(result)
    أرجع result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  أرجع CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    بنية Curl_cfilter *cf, بنية Curl_easy *data,
    بنية http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  بنية dynhds_entry *capsule_protocol = عدم;
  بنية dynhds_entry *auth_reply = عدم;
  size_t i, header_گنتی;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_گنتی = Curl_dynhds_گنتی(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_گنتی);
  لكل(i = 0; i < header_گنتی; i++) {
    بنية dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    إذا(entry)
      infof(data, "  %s: %s", entry->name, entry->قدر);
  }

  إذا(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  إذا(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->قدر);
    result = Curl_http_داخلہ_auth(data, resp->status == 407,
                                  auth_reply->قدر);
    إذا(result)
      أرجع result;
    إذا(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      أرجع CURLE_OK;
    }
  }

  إذا(udp_tunnel) {
    إذا(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      إذا(capsule_protocol) {
        إذا(!strncmp(capsule_protocol->قدر, "?1", 2) &&
           !capsule_protocol->قدر[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          أرجع CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->قدر);
        *presult = PROXY_INSPECT_FAILED;
        أرجع CURLE_COULDNT_CONNECT;
      }
      وإلا {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        أرجع CURLE_OK;
      }
    }
    وإلا {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  إذا(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    أرجع CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  أرجع CURLE_COULDNT_CONNECT;
}

ساكن CURLcode http_proxy_cf_connect(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      bool *done)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  إذا(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    إذا(result || !*done)
      أرجع result;
  }

  *done = FALSE;
  إذا(!ctx->sub_filter_installed) {
    ثابت حرف *alpn = عدم;

    /* in case of h3_proxy, cf->next will be NULL initially */
    إذا(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    إذا(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    وإلا إذا(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      بدل(ctx->proxytype) {
      حالة CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        اكسر;
      حالة CURLPROXY_HTTPS2:
        alpn = "h2";
        اكسر;
      حالة CURLPROXY_HTTPS3:
        alpn = "h3";
        اكسر;
      افتراضي:
        alpn = "http/1.1";
        اكسر;
      }
    }

    إذا(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_داخل_کرو_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      إذا(result)
        اذهب out;
    }
    وإلا إذا(!strcmp(alpn, "http/1.1")) {
      صحيح httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_داخل_کرو_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      إذا(result)
        اذهب out;
    }
#ifdef USE_NGHTTP2
    وإلا إذا(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_داخل_کرو_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      إذا(result)
        اذهب out;
    }
#endif /* USE_NGHTTP2 */
#إذا defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    وإلا إذا(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_داخل_کرو_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      إذا(result)
        اذهب out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    وإلا {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      اذهب out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    اذهب connect_sub;
  }
  وإلا {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  إذا(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  أرجع result;
}

ساكن CURLcode cf_http_proxy_query(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  بدل(query) {
  حالة CF_QUERY_HOST_PORT:
    *pres1 = (صحيح)ctx->tunnel_peer->port;
    *((ثابت حرف **)pres2) = ctx->tunnel_peer->hostname;
    أرجع CURLE_OK;
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = عدم;
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن فراغ cf_https_proxy_ctx_free(بنية cf_proxy_ctx *ctx)
{
  إذا(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ساكن فراغ http_proxy_cf_destroy(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  إذا(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

بنية Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                         بنية Curl_easy *data,
                                         بنية Curl_peer *peer,
                                         بنية Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  بنية Curl_cfilter *cf;
  بنية cf_proxy_ctx *ctx = عدم;
  CURLcode result;

  (فراغ)data;
  إذا(!peer || !tunnel_peer)
    أرجع CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  إذا(result)
    اذهب out;
  ctx = عدم;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  أرجع result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  بدل(proxytype) {
  حالة CURLPROXY_HTTPS3:
    أرجع TRNSPRT_QUIC;
  افتراضي:
    أرجع TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
