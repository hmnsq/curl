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

#ܐܢ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

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

  ܐܢ(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  ܐܚܪܢܐ ܐܢ(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  ܐܚܪܢܐ
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  بدل(proxy) {
  حالة HEADER_SERVER:
    h[0] = data->set.headers;
    اكسر;
  حالة HEADER_PROXY:
    h[0] = data->set.headers;
    ܐܢ(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة HEADER_CONNECT:
    ܐܢ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ܐܚܪܢܐ
      h[0] = data->set.headers;
    اكسر;
  حالة HEADER_CONNECT_UDP:
    ܐܢ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ܐܚܪܢܐ
      h[0] = data->set.headers;
    اكسر;
  }

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(headers = h[i]; headers; headers = headers->next) {
      بنية Curl_str name;
      ثابت حرف *value = عدم;
      size_t valuelen = 0;
      ثابت حرف *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ܐܢ(!curlx_str_cspn(&ptr, &name, ";:")) {
        ܐܢ(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ܐܢ(*ptr) {
            value = ptr;
            valuelen = strlen(value);
          }
          ܐܚܪܢܐ {
            /* quirk #1, suppress this header */
            استمر;
          }
        }
        ܐܚܪܢܐ ܐܢ(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ܐܢ(!*ptr) {
            /* quirk #2, send an empty header */
            value = "";
            valuelen = 0;
          }
          ܐܚܪܢܐ {
            /* this may be used for something else in the future,
             * ignore this for now */
            استمر;
          }
        }
        ܐܚܪܢܐ
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          استمر;
      }
      ܐܚܪܢܐ
        /* no name, move on */
        استمر;

      DEBUGASSERT(curlx_strlen(&name) && value);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ܐܢ(!curlx_strlen(&name))
        استمر;
      ܐܢ(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ܐܚܪܢܐ ܐܢ(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ܐܚܪܢܐ ܐܢ(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ܐܚܪܢܐ ܐܢ(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ܐܚܪܢܐ ܐܢ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      ܐܚܪܢܐ ܐܢ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      ܐܚܪܢܐ {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          value, valuelen);
        ܐܢ(result)
          ܦܢܐ result;
      }
    }
  }

  ܦܢܐ CURLE_OK;
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
    ܦܢܐ 11;
  حالة PROXY_HTTP_V2:
    ܦܢܐ 20;
  حالة PROXY_HTTP_V3:
    ܦܢܐ 30;
  }
  ܦܢܐ 0;
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
  ܐܢ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              عدم, 0, authority, strlen(authority),
                              عدم, 0);
  ܐܢ(result)
    اذهب out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  ܐܢ(result)
    اذهب out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ܐܢ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ܐܢ(result)
      اذهب out;
  }

  ܐܢ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    ܐܢ(result)
      اذهب out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ܐܢ(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ܐܢ(result)
      اذهب out;
  }

  ܐܢ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ܐܢ(result)
      اذهب out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  ܐܢ(result && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  *preq = req;
  ܦܢܐ result;
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

  ܐܢ(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  ܐܢ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  ܐܢ(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ثابت حرف *s = dest->hostname;
    حرف *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ܐܢ(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
    d = encoded_host;
    ܟܡܐ(*s) {
      ܐܢ(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      ܐܚܪܢܐ
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned صحيح)dest->port);
  }
  ܐܚܪܢܐ {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned صحيح)dest->port);
  }

  ܐܢ(!path) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  ܐܢ(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ܐܢ(result)
      اذهب out;
  }
  ܐܚܪܢܐ ܐܢ(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ܐܢ(result)
      اذهب out;
  }
  ܐܚܪܢܐ {
    result = CURLE_FAILED_INIT;
    اذهب out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  ܐܢ(result)
    اذهب out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ܐܢ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ܐܢ(result)
      اذهب out;
  }

  ܐܢ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    ܐܢ(result)
      اذهب out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ܐܢ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ܐܢ(result)
      اذهب out;
  }

  ܐܢ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ܐܢ(result)
      اذهب out;
  }

  ܐܢ(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    ܐܢ(result)
      اذهب out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    ܐܢ(result)
      اذهب out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    ܐܢ(result)
      اذهب out;
  }
  ܐܚܪܢܐ {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    ܐܢ(result)
      اذهب out;

    ܐܢ(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      ܐܢ(result)
        اذهب out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  ܐܢ(result && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  ܦܢܐ result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    بنية httpreq **preq, بنية Curl_cfilter *cf,
    بنية Curl_easy *data, بنية Curl_peer *peer, بنية Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  ܐܢ(result)
    ܦܢܐ result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  ܦܢܐ CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    بنية Curl_cfilter *cf, بنية Curl_easy *data,
    بنية http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  بنية dynhds_entry *capsule_protocol = عدم;
  بنية dynhds_entry *auth_reply = عدم;
  size_t i, header_count;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_count = Curl_dynhds_count(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_count);
  لكل(i = 0; i < header_count; i++) {
    بنية dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    ܐܢ(entry)
      infof(data, "  %s: %s", entry->name, entry->value);
  }

  ܐܢ(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  ܐܢ(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->value);
    result = Curl_http_input_auth(data, resp->status == 407,
                                  auth_reply->value);
    ܐܢ(result)
      ܦܢܐ result;
    ܐܢ(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      ܦܢܐ CURLE_OK;
    }
  }

  ܐܢ(udp_tunnel) {
    ܐܢ(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      ܐܢ(capsule_protocol) {
        ܐܢ(!strncmp(capsule_protocol->value, "?1", 2) &&
           !capsule_protocol->value[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          ܦܢܐ CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->value);
        *presult = PROXY_INSPECT_FAILED;
        ܦܢܐ CURLE_COULDNT_CONNECT;
      }
      ܐܚܪܢܐ {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        ܦܢܐ CURLE_OK;
      }
    }
    ܐܚܪܢܐ {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      ܦܢܐ CURLE_COULDNT_CONNECT;
    }
  }

  ܐܢ(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    ܦܢܐ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  ܦܢܐ CURLE_COULDNT_CONNECT;
}

ساكن CURLcode http_proxy_cf_connect(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      bool *done)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ܐܢ(cf->connected) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ܐܢ(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ܐܢ(result || !*done)
      ܦܢܐ result;
  }

  *done = FALSE;
  ܐܢ(!ctx->sub_filter_installed) {
    ثابت حرف *alpn = عدم;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ܐܢ(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    ܐܢ(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    ܐܚܪܢܐ ܐܢ(!alpn) {
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

    ܐܢ(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ܐܢ(result)
        اذهب out;
    }
    ܐܚܪܢܐ ܐܢ(!strcmp(alpn, "http/1.1")) {
      صحيح httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ܐܢ(result)
        اذهب out;
    }
#ifdef USE_NGHTTP2
    ܐܚܪܢܐ ܐܢ(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ܐܢ(result)
        اذهب out;
    }
#endif /* USE_NGHTTP2 */
#ܐܢ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    ܐܚܪܢܐ ܐܢ(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_insert_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ܐܢ(result)
        اذهب out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    ܐܚܪܢܐ {
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
  ܐܚܪܢܐ {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  ܐܢ(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  ܦܢܐ result;
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
    ܦܢܐ CURLE_OK;
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = عدم;
    ܦܢܐ CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  ܦܢܐ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن فراغ cf_https_proxy_ctx_free(بنية cf_proxy_ctx *ctx)
{
  ܐܢ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ساكن فراغ http_proxy_cf_destroy(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  ܐܢ(ctx) {
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

CURLcode Curl_cf_http_proxy_insert_after(بنية Curl_cfilter *cf_at,
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
  ܐܢ(!peer || !tunnel_peer)
    ܦܢܐ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  ܐܢ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ܐܢ(result)
    اذهب out;
  ctx = عدم;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  ܦܢܐ result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  بدل(proxytype) {
  حالة CURLPROXY_HTTPS3:
    ܦܢܐ TRNSPRT_QUIC;
  افتراضي:
    ܦܢܐ TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
