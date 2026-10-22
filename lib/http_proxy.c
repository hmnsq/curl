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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "http_proxy.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-h1-proxy.h"
#அந்தர்பாவ "cf-h2-proxy.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "curlx/strparse.h"

நிலையான CURLcode dynhds_add_custom(கட்டமைப்பு Curl_easy *data,
                                  bool is_connect, முழு httpversion,
                                  bool is_udp, கட்டமைப்பு dynhds *hds)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு curl_slist *h[2];
  கட்டமைப்பு curl_slist *headers;
  முழு numlists = 1; /* by default */
  முழு i;

  எண்ணிக்கை Curl_proxy_use proxy;

  எனில்(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  இல்லையெனில் எனில்(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  இல்லையெனில்
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  தேர்வு(proxy) {
  நிலை HEADER_SERVER:
    h[0] = data->set.headers;
    நிறுத்து;
  நிலை HEADER_PROXY:
    h[0] = data->set.headers;
    எனில்(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    நிறுத்து;
  நிலை HEADER_CONNECT:
    எனில்(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    இல்லையெனில்
      h[0] = data->set.headers;
    நிறுத்து;
  நிலை HEADER_CONNECT_UDP:
    எனில்(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    இல்லையெனில்
      h[0] = data->set.headers;
    நிறுத்து;
  }

  /* loop through one or two lists */
  க்கு(i = 0; i < numlists; i++) {
    க்கு(headers = h[i]; headers; headers = headers->next) {
      கட்டமைப்பு Curl_str name;
      மாறிலி எழுத்து *மதிப்பு = வெற்று;
      size_t valuelen = 0;
      மாறிலி எழுத்து *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      எனில்(!curlx_str_cspn(&ptr, &name, ";:")) {
        எனில்(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          எனில்(*ptr) {
            மதிப்பு = ptr;
            valuelen = strlen(மதிப்பு);
          }
          இல்லையெனில் {
            /* quirk #1, suppress this header */
            தொடர்;
          }
        }
        இல்லையெனில் எனில்(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          எனில்(!*ptr) {
            /* quirk #2, send an empty header */
            மதிப்பு = "";
            valuelen = 0;
          }
          இல்லையெனில் {
            /* this may be used for something else in the future,
             * ignore this for now */
            தொடர்;
          }
        }
        இல்லையெனில்
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          தொடர்;
      }
      இல்லையெனில்
        /* no name, move on */
        தொடர்;

      DEBUGASSERT(curlx_strlen(&name) && மதிப்பு);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      எனில்(!curlx_strlen(&name))
        தொடர்;
      எனில்(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      இல்லையெனில் எனில்(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      இல்லையெனில் எனில்(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      இல்லையெனில் எனில்(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      இல்லையெனில் எனில்((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      இல்லையெனில் எனில்((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      இல்லையெனில் {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          மதிப்பு, valuelen);
        எனில்(result)
          திரும்பு result;
      }
    }
  }

  திரும்பு CURLE_OK;
}

கட்டமைப்பு cf_proxy_ctx {
  கட்டமைப்பு Curl_peer *peer; /* proxy */
  கட்டமைப்பு Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

நிலையான முழு proxy_http_ver_major(proxy_http_ver ver)
{
  தேர்வு(ver) {
  நிலை PROXY_HTTP_V1:
    திரும்பு 11;
  நிலை PROXY_HTTP_V2:
    திரும்பு 20;
  நிலை PROXY_HTTP_V3:
    திரும்பு 30;
  }
  திரும்பு 0;
}

நிலையான CURLcode http_proxy_create_CONNECT(கட்டமைப்பு httpreq **preq,
                                          கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு Curl_peer *peer,
                                          கட்டமைப்பு Curl_peer *dest,
                                          proxy_http_ver ver)
{
  எழுத்து *authority = வெற்று;
  மாறிலி எழுத்து *ua;
  முழு httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  கட்டமைப்பு httpreq *req = வெற்று;

  (வெறுமை)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  எனில்(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              வெற்று, 0, authority, strlen(authority),
                              வெற்று, 0);
  எனில்(result)
    செல் out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, வெற்று, TRUE);
  எனில்(result)
    செல் out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  எனில்(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    எனில்(result)
      செல் out;
  }

  எனில்(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_வரி(&req->headers,
                                      data->req.hd_proxy_auth);
    எனில்(result)
      செல் out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  எனில்(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    எனில்(result)
      செல் out;
  }

  எனில்(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    எனில்(result)
      செல் out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  எனில்(result && req) {
    Curl_http_req_free(req);
    req = வெற்று;
  }
  curlx_free(authority);
  *preq = req;
  திரும்பு result;
}

நிலையான CURLcode http_proxy_create_CONNECTUDP(கட்டமைப்பு httpreq **preq,
                                             கட்டமைப்பு Curl_cfilter *cf,
                                             கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு Curl_peer *peer,
                                             கட்டமைப்பு Curl_peer *dest,
                                             proxy_http_ver ver)
{
  மாறிலி எழுத்து *proxy_scheme = "http", *ua;
  முழு httpversion = proxy_http_ver_major(ver);
  எழுத்து *authority = வெற்று;
  எழுத்து *path = வெற்று;
  எழுத்து *encoded_host = வெற்று;
  கட்டமைப்பு httpreq *req = வெற்று;
  CURLcode result;

  எனில்(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  எனில்(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  எனில்(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    மாறிலி எழுத்து *s = dest->hostname;
    எழுத்து *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    எனில்(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
    d = encoded_host;
    போது(*s) {
      எனில்(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      இல்லையெனில்
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned முழு)dest->port);
  }
  இல்லையெனில் {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned முழு)dest->port);
  }

  எனில்(!path) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  எனில்(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    எனில்(result)
      செல் out;
  }
  இல்லையெனில் எனில்(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    எனில்(result)
      செல் out;
  }
  இல்லையெனில் {
    result = CURLE_FAILED_INIT;
    செல் out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, வெற்று, TRUE);
  எனில்(result)
    செல் out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  எனில்(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    எனில்(result)
      செல் out;
  }

  எனில்(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_வரி(&req->headers,
                                      data->req.hd_proxy_auth);
    எனில்(result)
      செல் out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  எனில்(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    எனில்(result)
      செல் out;
  }

  எனில்(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    எனில்(result)
      செல் out;
  }

  எனில்(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    எனில்(result)
      செல் out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    எனில்(result)
      செல் out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    எனில்(result)
      செல் out;
  }
  இல்லையெனில் {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    எனில்(result)
      செல் out;

    எனில்(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      எனில்(result)
        செல் out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  எனில்(result && req) {
    Curl_http_req_free(req);
    req = வெற்று;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  திரும்பு result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    கட்டமைப்பு httpreq **preq, கட்டமைப்பு Curl_cfilter *cf,
    கட்டமைப்பு Curl_easy *data, கட்டமைப்பு Curl_peer *peer, கட்டமைப்பு Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  எனில்(result)
    திரும்பு result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  திரும்பு CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
    கட்டமைப்பு http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  கட்டமைப்பு dynhds_entry *capsule_protocol = வெற்று;
  கட்டமைப்பு dynhds_entry *auth_reply = வெற்று;
  size_t i, header_மொத்தம்;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_மொத்தம் = Curl_dynhds_மொத்தம்(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_மொத்தம்);
  க்கு(i = 0; i < header_மொத்தம்; i++) {
    கட்டமைப்பு dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    எனில்(entry)
      infof(data, "  %s: %s", entry->name, entry->மதிப்பு);
  }

  எனில்(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  எனில்(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->மதிப்பு);
    result = Curl_http_உள்ளீடு_auth(data, resp->status == 407,
                                  auth_reply->மதிப்பு);
    எனில்(result)
      திரும்பு result;
    எனில்(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      திரும்பு CURLE_OK;
    }
  }

  எனில்(udp_tunnel) {
    எனில்(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      எனில்(capsule_protocol) {
        எனில்(!strncmp(capsule_protocol->மதிப்பு, "?1", 2) &&
           !capsule_protocol->மதிப்பு[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          திரும்பு CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->மதிப்பு);
        *presult = PROXY_INSPECT_FAILED;
        திரும்பு CURLE_COULDNT_CONNECT;
      }
      இல்லையெனில் {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        திரும்பு CURLE_OK;
      }
    }
    இல்லையெனில் {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      திரும்பு CURLE_COULDNT_CONNECT;
    }
  }

  எனில்(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    திரும்பு CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  திரும்பு CURLE_COULDNT_CONNECT;
}

நிலையான CURLcode http_proxy_cf_connect(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      bool *done)
{
  கட்டமைப்பு cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  எனில்(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  *done = FALSE;
  எனில்(!ctx->sub_filter_installed) {
    மாறிலி எழுத்து *alpn = வெற்று;

    /* in case of h3_proxy, cf->next will be NULL initially */
    எனில்(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    எனில்(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    இல்லையெனில் எனில்(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      தேர்வு(ctx->proxytype) {
      நிலை CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        நிறுத்து;
      நிலை CURLPROXY_HTTPS2:
        alpn = "h2";
        நிறுத்து;
      நிலை CURLPROXY_HTTPS3:
        alpn = "h3";
        நிறுத்து;
      இயல்பு:
        alpn = "http/1.1";
        நிறுத்து;
      }
    }

    எனில்(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_செருகு_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      எனில்(result)
        செல் out;
    }
    இல்லையெனில் எனில்(!strcmp(alpn, "http/1.1")) {
      முழு httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_செருகு_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      எனில்(result)
        செல் out;
    }
#ifdef USE_NGHTTP2
    இல்லையெனில் எனில்(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_செருகு_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      எனில்(result)
        செல் out;
    }
#endif /* USE_NGHTTP2 */
#எனில் defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    இல்லையெனில் எனில்(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_செருகு_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      எனில்(result)
        செல் out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    இல்லையெனில் {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      செல் out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    செல் connect_sub;
  }
  இல்லையெனில் {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  எனில்(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_http_proxy_query(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_proxy_ctx *ctx = cf->ctx;
  தேர்வு(query) {
  நிலை CF_QUERY_HOST_PORT:
    *pres1 = (முழு)ctx->tunnel_peer->port;
    *((மாறிலி எழுத்து **)pres2) = ctx->tunnel_peer->hostname;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_ALPN_NEGOTIATED: {
    மாறிலி எழுத்து **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = வெற்று;
    திரும்பு CURLE_OK;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

நிலையான வெறுமை cf_https_proxy_ctx_free(கட்டமைப்பு cf_proxy_ctx *ctx)
{
  எனில்(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

நிலையான வெறுமை http_proxy_cf_destroy(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_proxy_ctx *ctx = cf->ctx;
  எனில்(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

கட்டமைப்பு Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                         கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு Curl_peer *peer,
                                         கட்டமைப்பு Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு cf_proxy_ctx *ctx = வெற்று;
  CURLcode result;

  (வெறுமை)data;
  எனில்(!peer || !tunnel_peer)
    திரும்பு CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  எனில்(result)
    செல் out;
  ctx = வெற்று;
  Curl_conn_cf_செருகு_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  திரும்பு result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  தேர்வு(proxytype) {
  நிலை CURLPROXY_HTTPS3:
    திரும்பு TRNSPRT_QUIC;
  இயல்பு:
    திரும்பு TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
