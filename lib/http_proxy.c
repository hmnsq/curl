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

#অন্তর্ভাব "http_proxy.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-h1-proxy.h"
#অন্তর্ভাব "cf-h2-proxy.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "curlx/strparse.h"

স্থির CURLcode dynhds_add_custom(সংরচনা Curl_easy *data,
                                  bool is_connect, পূর্ণাঙ্ক httpversion,
                                  bool is_udp, সংরচনা dynhds *hds)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা curl_slist *h[2];
  সংরচনা curl_slist *headers;
  পূর্ণাঙ্ক numlists = 1; /* by default */
  পূর্ণাঙ্ক i;

  গণনা Curl_proxy_use proxy;

  যদি(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  অন্যথা যদি(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  অন্যথা
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  বিকল্প(proxy) {
  পক্ষ HEADER_SERVER:
    h[0] = data->set.headers;
    ভঙ্গ;
  পক্ষ HEADER_PROXY:
    h[0] = data->set.headers;
    যদি(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    ভঙ্গ;
  পক্ষ HEADER_CONNECT:
    যদি(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    অন্যথা
      h[0] = data->set.headers;
    ভঙ্গ;
  পক্ষ HEADER_CONNECT_UDP:
    যদি(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    অন্যথা
      h[0] = data->set.headers;
    ভঙ্গ;
  }

  /* loop through one or two lists */
  প্রতি(i = 0; i < numlists; i++) {
    প্রতি(headers = h[i]; headers; headers = headers->next) {
      সংরচনা Curl_str name;
      ধ্রুব বর্ণ *মান = রিক্ত;
      size_t valuelen = 0;
      ধ্রুব বর্ণ *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      যদি(!curlx_str_cspn(&ptr, &name, ";:")) {
        যদি(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          যদি(*ptr) {
            মান = ptr;
            valuelen = strlen(মান);
          }
          অন্যথা {
            /* quirk #1, suppress this header */
            অনুবর্ত;
          }
        }
        অন্যথা যদি(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          যদি(!*ptr) {
            /* quirk #2, send an empty header */
            মান = "";
            valuelen = 0;
          }
          অন্যথা {
            /* this may be used for something else in the future,
             * ignore this for now */
            অনুবর্ত;
          }
        }
        অন্যথা
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          অনুবর্ত;
      }
      অন্যথা
        /* no name, move on */
        অনুবর্ত;

      DEBUGASSERT(curlx_strlen(&name) && মান);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      যদি(!curlx_strlen(&name))
        অনুবর্ত;
      যদি(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      অন্যথা যদি(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      অন্যথা যদি((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      অন্যথা যদি((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      অন্যথা {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          মান, valuelen);
        যদি(result)
          প্রত্যাগম result;
      }
    }
  }

  প্রত্যাগম CURLE_OK;
}

সংরচনা cf_proxy_ctx {
  সংরচনা Curl_peer *peer; /* proxy */
  সংরচনা Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

স্থির পূর্ণাঙ্ক proxy_http_ver_major(proxy_http_ver ver)
{
  বিকল্প(ver) {
  পক্ষ PROXY_HTTP_V1:
    প্রত্যাগম 11;
  পক্ষ PROXY_HTTP_V2:
    প্রত্যাগম 20;
  পক্ষ PROXY_HTTP_V3:
    প্রত্যাগম 30;
  }
  প্রত্যাগম 0;
}

স্থির CURLcode http_proxy_create_CONNECT(সংরচনা httpreq **preq,
                                          সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          সংরচনা Curl_peer *peer,
                                          সংরচনা Curl_peer *dest,
                                          proxy_http_ver ver)
{
  বর্ণ *authority = রিক্ত;
  ধ্রুব বর্ণ *ua;
  পূর্ণাঙ্ক httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  সংরচনা httpreq *req = রিক্ত;

  (শূন্য)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  যদি(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              রিক্ত, 0, authority, strlen(authority),
                              রিক্ত, 0);
  যদি(result)
    গচ্ছ out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, রিক্ত, TRUE);
  যদি(result)
    গচ্ছ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    যদি(result)
      গচ্ছ out;
  }

  যদি(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_সারি(&req->headers,
                                      data->req.hd_proxy_auth);
    যদি(result)
      গচ্ছ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  যদি(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    যদি(result)
      গচ্ছ out;
  }

  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    যদি(result)
      গচ্ছ out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  যদি(result && req) {
    Curl_http_req_free(req);
    req = রিক্ত;
  }
  curlx_free(authority);
  *preq = req;
  প্রত্যাগম result;
}

স্থির CURLcode http_proxy_create_CONNECTUDP(সংরচনা httpreq **preq,
                                             সংরচনা Curl_cfilter *cf,
                                             সংরচনা Curl_easy *data,
                                             সংরচনা Curl_peer *peer,
                                             সংরচনা Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ধ্রুব বর্ণ *proxy_scheme = "http", *ua;
  পূর্ণাঙ্ক httpversion = proxy_http_ver_major(ver);
  বর্ণ *authority = রিক্ত;
  বর্ণ *path = রিক্ত;
  বর্ণ *encoded_host = রিক্ত;
  সংরচনা httpreq *req = রিক্ত;
  CURLcode result;

  যদি(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  যদি(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  যদি(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ধ্রুব বর্ণ *s = dest->hostname;
    বর্ণ *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    যদি(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
    d = encoded_host;
    যাবত্(*s) {
      যদি(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      অন্যথা
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned পূর্ণাঙ্ক)dest->port);
  }
  অন্যথা {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned পূর্ণাঙ্ক)dest->port);
  }

  যদি(!path) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  যদি(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা যদি(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা {
    result = CURLE_FAILED_INIT;
    গচ্ছ out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, রিক্ত, TRUE);
  যদি(result)
    গচ্ছ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    যদি(result)
      গচ্ছ out;
  }

  যদি(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_সারি(&req->headers,
                                      data->req.hd_proxy_auth);
    যদি(result)
      গচ্ছ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    যদি(result)
      গচ্ছ out;
  }

  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    যদি(result)
      গচ্ছ out;
  }

  যদি(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    যদি(result)
      গচ্ছ out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    যদি(result)
      গচ্ছ out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    যদি(result)
      গচ্ছ out;

    যদি(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      যদি(result)
        গচ্ছ out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  যদি(result && req) {
    Curl_http_req_free(req);
    req = রিক্ত;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  প্রত্যাগম result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    সংরচনা httpreq **preq, সংরচনা Curl_cfilter *cf,
    সংরচনা Curl_easy *data, সংরচনা Curl_peer *peer, সংরচনা Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  যদি(result)
    প্রত্যাগম result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
    সংরচনা http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  সংরচনা dynhds_entry *capsule_protocol = রিক্ত;
  সংরচনা dynhds_entry *auth_reply = রিক্ত;
  size_t i, header_গুনতি;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_গুনতি = Curl_dynhds_গুনতি(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_গুনতি);
  প্রতি(i = 0; i < header_গুনতি; i++) {
    সংরচনা dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    যদি(entry)
      infof(data, "  %s: %s", entry->name, entry->মান);
  }

  যদি(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  যদি(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->মান);
    result = Curl_http_প্রবেশ_auth(data, resp->status == 407,
                                  auth_reply->মান);
    যদি(result)
      প্রত্যাগম result;
    যদি(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      প্রত্যাগম CURLE_OK;
    }
  }

  যদি(udp_tunnel) {
    যদি(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      যদি(capsule_protocol) {
        যদি(!strncmp(capsule_protocol->মান, "?1", 2) &&
           !capsule_protocol->মান[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          প্রত্যাগম CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->মান);
        *presult = PROXY_INSPECT_FAILED;
        প্রত্যাগম CURLE_COULDNT_CONNECT;
      }
      অন্যথা {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        প্রত্যাগম CURLE_OK;
      }
    }
    অন্যথা {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }
  }

  যদি(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    প্রত্যাগম CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  প্রত্যাগম CURLE_COULDNT_CONNECT;
}

স্থির CURLcode http_proxy_cf_connect(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      bool *done)
{
  সংরচনা cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  যদি(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    যদি(result || !*done)
      প্রত্যাগম result;
  }

  *done = FALSE;
  যদি(!ctx->sub_filter_installed) {
    ধ্রুব বর্ণ *alpn = রিক্ত;

    /* in case of h3_proxy, cf->next will be NULL initially */
    যদি(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    যদি(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    অন্যথা যদি(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      বিকল্প(ctx->proxytype) {
      পক্ষ CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        ভঙ্গ;
      পক্ষ CURLPROXY_HTTPS2:
        alpn = "h2";
        ভঙ্গ;
      পক্ষ CURLPROXY_HTTPS3:
        alpn = "h3";
        ভঙ্গ;
      স্বভাব:
        alpn = "http/1.1";
        ভঙ্গ;
      }
    }

    যদি(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_ঢোকাও_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      যদি(result)
        গচ্ছ out;
    }
    অন্যথা যদি(!strcmp(alpn, "http/1.1")) {
      পূর্ণাঙ্ক httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_ঢোকাও_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      যদি(result)
        গচ্ছ out;
    }
#ifdef USE_NGHTTP2
    অন্যথা যদি(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_ঢোকাও_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      যদি(result)
        গচ্ছ out;
    }
#endif /* USE_NGHTTP2 */
#যদি defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    অন্যথা যদি(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_ঢোকাও_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      যদি(result)
        গচ্ছ out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    অন্যথা {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      গচ্ছ out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    গচ্ছ connect_sub;
  }
  অন্যথা {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  যদি(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  প্রত্যাগম result;
}

স্থির CURLcode cf_http_proxy_query(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_proxy_ctx *ctx = cf->ctx;
  বিকল্প(query) {
  পক্ষ CF_QUERY_HOST_PORT:
    *pres1 = (পূর্ণাঙ্ক)ctx->tunnel_peer->port;
    *((ধ্রুব বর্ণ **)pres2) = ctx->tunnel_peer->hostname;
    প্রত্যাগম CURLE_OK;
  পক্ষ CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুব বর্ণ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = রিক্ত;
    প্রত্যাগম CURLE_OK;
  }
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির শূন্য cf_https_proxy_ctx_free(সংরচনা cf_proxy_ctx *ctx)
{
  যদি(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

স্থির শূন্য http_proxy_cf_destroy(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা cf_proxy_ctx *ctx = cf->ctx;
  যদি(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

সংরচনা Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                         সংরচনা Curl_easy *data,
                                         সংরচনা Curl_peer *peer,
                                         সংরচনা Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  সংরচনা Curl_cfilter *cf;
  সংরচনা cf_proxy_ctx *ctx = রিক্ত;
  CURLcode result;

  (শূন্য)data;
  যদি(!peer || !tunnel_peer)
    প্রত্যাগম CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  যদি(result)
    গচ্ছ out;
  ctx = রিক্ত;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  প্রত্যাগম result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  বিকল্প(proxytype) {
  পক্ষ CURLPROXY_HTTPS3:
    প্রত্যাগম TRNSPRT_QUIC;
  স্বভাব:
    প্রত্যাগম TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
