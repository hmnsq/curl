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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "http_proxy.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-h1-proxy.h"
#അന്തര്ഭാവ "cf-h2-proxy.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "curlx/strparse.h"

സ്ഥിതം CURLcode dynhds_add_custom(ഘടന Curl_easy *data,
                                  bool is_connect, പൂർണ്ണസംഖ്യ httpversion,
                                  bool is_udp, ഘടന dynhds *hds)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന curl_slist *h[2];
  ഘടന curl_slist *headers;
  പൂർണ്ണസംഖ്യ numlists = 1; /* by default */
  പൂർണ്ണസംഖ്യ i;

  എണ്ണൽ Curl_proxy_use proxy;

  എങ്കിൽ(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  അല്ലെങ്കിൽ എങ്കിൽ(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  അല്ലെങ്കിൽ
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  തിരഞ്ഞെടുപ്പ്(proxy) {
  അവസ്ഥ HEADER_SERVER:
    h[0] = data->set.headers;
    നിർത്തുക;
  അവസ്ഥ HEADER_PROXY:
    h[0] = data->set.headers;
    എങ്കിൽ(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    നിർത്തുക;
  അവസ്ഥ HEADER_CONNECT:
    എങ്കിൽ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    അല്ലെങ്കിൽ
      h[0] = data->set.headers;
    നിർത്തുക;
  അവസ്ഥ HEADER_CONNECT_UDP:
    എങ്കിൽ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    അല്ലെങ്കിൽ
      h[0] = data->set.headers;
    നിർത്തുക;
  }

  /* loop through one or two lists */
  വേണ്ടി(i = 0; i < numlists; i++) {
    വേണ്ടി(headers = h[i]; headers; headers = headers->next) {
      ഘടന Curl_str name;
      സ്ഥിരം അക്ഷരം *മൂല്യം = ശൂന്യ;
      size_t valuelen = 0;
      സ്ഥിരം അക്ഷരം *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      എങ്കിൽ(!curlx_str_cspn(&ptr, &name, ";:")) {
        എങ്കിൽ(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          എങ്കിൽ(*ptr) {
            മൂല്യം = ptr;
            valuelen = strlen(മൂല്യം);
          }
          അല്ലെങ്കിൽ {
            /* quirk #1, suppress this header */
            തുടരുക;
          }
        }
        അല്ലെങ്കിൽ എങ്കിൽ(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          എങ്കിൽ(!*ptr) {
            /* quirk #2, send an empty header */
            മൂല്യം = "";
            valuelen = 0;
          }
          അല്ലെങ്കിൽ {
            /* this may be used for something else in the future,
             * ignore this for now */
            തുടരുക;
          }
        }
        അല്ലെങ്കിൽ
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          തുടരുക;
      }
      അല്ലെങ്കിൽ
        /* no name, move on */
        തുടരുക;

      DEBUGASSERT(curlx_strlen(&name) && മൂല്യം);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      എങ്കിൽ(!curlx_strlen(&name))
        തുടരുക;
      എങ്കിൽ(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      അല്ലെങ്കിൽ എങ്കിൽ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      അല്ലെങ്കിൽ എങ്കിൽ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      അല്ലെങ്കിൽ {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          മൂല്യം, valuelen);
        എങ്കിൽ(result)
          തിരികെ result;
      }
    }
  }

  തിരികെ CURLE_OK;
}

ഘടന cf_proxy_ctx {
  ഘടന Curl_peer *peer; /* proxy */
  ഘടന Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

സ്ഥിതം പൂർണ്ണസംഖ്യ proxy_http_ver_major(proxy_http_ver ver)
{
  തിരഞ്ഞെടുപ്പ്(ver) {
  അവസ്ഥ PROXY_HTTP_V1:
    തിരികെ 11;
  അവസ്ഥ PROXY_HTTP_V2:
    തിരികെ 20;
  അവസ്ഥ PROXY_HTTP_V3:
    തിരികെ 30;
  }
  തിരികെ 0;
}

സ്ഥിതം CURLcode http_proxy_create_CONNECT(ഘടന httpreq **preq,
                                          ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          ഘടന Curl_peer *peer,
                                          ഘടന Curl_peer *dest,
                                          proxy_http_ver ver)
{
  അക്ഷരം *authority = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *ua;
  പൂർണ്ണസംഖ്യ httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  ഘടന httpreq *req = ശൂന്യ;

  (ശൂന്യം)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  എങ്കിൽ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ശൂന്യ, 0, authority, strlen(authority),
                              ശൂന്യ, 0);
  എങ്കിൽ(result)
    പോകുക out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ശൂന്യ, TRUE);
  എങ്കിൽ(result)
    പോകുക out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  എങ്കിൽ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_വരി(&req->headers,
                                      data->req.hd_proxy_auth);
    എങ്കിൽ(result)
      പോകുക out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  എങ്കിൽ(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    എങ്കിൽ(result)
      പോകുക out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  എങ്കിൽ(result && req) {
    Curl_http_req_free(req);
    req = ശൂന്യ;
  }
  curlx_free(authority);
  *preq = req;
  തിരികെ result;
}

സ്ഥിതം CURLcode http_proxy_create_CONNECTUDP(ഘടന httpreq **preq,
                                             ഘടന Curl_cfilter *cf,
                                             ഘടന Curl_easy *data,
                                             ഘടന Curl_peer *peer,
                                             ഘടന Curl_peer *dest,
                                             proxy_http_ver ver)
{
  സ്ഥിരം അക്ഷരം *proxy_scheme = "http", *ua;
  പൂർണ്ണസംഖ്യ httpversion = proxy_http_ver_major(ver);
  അക്ഷരം *authority = ശൂന്യ;
  അക്ഷരം *path = ശൂന്യ;
  അക്ഷരം *encoded_host = ശൂന്യ;
  ഘടന httpreq *req = ശൂന്യ;
  CURLcode result;

  എങ്കിൽ(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  എങ്കിൽ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  എങ്കിൽ(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    സ്ഥിരം അക്ഷരം *s = dest->hostname;
    അക്ഷരം *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    എങ്കിൽ(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
    d = encoded_host;
    ആകുമ്പോൾ(*s) {
      എങ്കിൽ(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      അല്ലെങ്കിൽ
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned പൂർണ്ണസംഖ്യ)dest->port);
  }
  അല്ലെങ്കിൽ {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned പൂർണ്ണസംഖ്യ)dest->port);
  }

  എങ്കിൽ(!path) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  എങ്കിൽ(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ {
    result = CURLE_FAILED_INIT;
    പോകുക out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ശൂന്യ, TRUE);
  എങ്കിൽ(result)
    പോകുക out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  എങ്കിൽ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_വരി(&req->headers,
                                      data->req.hd_proxy_auth);
    എങ്കിൽ(result)
      പോകുക out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  എങ്കിൽ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    എങ്കിൽ(result)
      പോകുക out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    എങ്കിൽ(result)
      പോകുക out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    എങ്കിൽ(result)
      പോകുക out;

    എങ്കിൽ(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      എങ്കിൽ(result)
        പോകുക out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  എങ്കിൽ(result && req) {
    Curl_http_req_free(req);
    req = ശൂന്യ;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  തിരികെ result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    ഘടന httpreq **preq, ഘടന Curl_cfilter *cf,
    ഘടന Curl_easy *data, ഘടന Curl_peer *peer, ഘടന Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  എങ്കിൽ(result)
    തിരികെ result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  തിരികെ CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
    ഘടന http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  ഘടന dynhds_entry *capsule_protocol = ശൂന്യ;
  ഘടന dynhds_entry *auth_reply = ശൂന്യ;
  size_t i, header_എണ്ണം;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_എണ്ണം = Curl_dynhds_എണ്ണം(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_എണ്ണം);
  വേണ്ടി(i = 0; i < header_എണ്ണം; i++) {
    ഘടന dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    എങ്കിൽ(entry)
      infof(data, "  %s: %s", entry->name, entry->മൂല്യം);
  }

  എങ്കിൽ(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  എങ്കിൽ(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->മൂല്യം);
    result = Curl_http_നിവേശം_auth(data, resp->status == 407,
                                  auth_reply->മൂല്യം);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      തിരികെ CURLE_OK;
    }
  }

  എങ്കിൽ(udp_tunnel) {
    എങ്കിൽ(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      എങ്കിൽ(capsule_protocol) {
        എങ്കിൽ(!strncmp(capsule_protocol->മൂല്യം, "?1", 2) &&
           !capsule_protocol->മൂല്യം[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          തിരികെ CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->മൂല്യം);
        *presult = PROXY_INSPECT_FAILED;
        തിരികെ CURLE_COULDNT_CONNECT;
      }
      അല്ലെങ്കിൽ {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        തിരികെ CURLE_OK;
      }
    }
    അല്ലെങ്കിൽ {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      തിരികെ CURLE_COULDNT_CONNECT;
    }
  }

  എങ്കിൽ(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    തിരികെ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  തിരികെ CURLE_COULDNT_CONNECT;
}

സ്ഥിതം CURLcode http_proxy_cf_connect(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      bool *done)
{
  ഘടന cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  എങ്കിൽ(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  *done = FALSE;
  എങ്കിൽ(!ctx->sub_filter_installed) {
    സ്ഥിരം അക്ഷരം *alpn = ശൂന്യ;

    /* in case of h3_proxy, cf->next will be NULL initially */
    എങ്കിൽ(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    എങ്കിൽ(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    അല്ലെങ്കിൽ എങ്കിൽ(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      തിരഞ്ഞെടുപ്പ്(ctx->proxytype) {
      അവസ്ഥ CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        നിർത്തുക;
      അവസ്ഥ CURLPROXY_HTTPS2:
        alpn = "h2";
        നിർത്തുക;
      അവസ്ഥ CURLPROXY_HTTPS3:
        alpn = "h3";
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        alpn = "http/1.1";
        നിർത്തുക;
      }
    }

    എങ്കിൽ(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_തിരുകുക_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      എങ്കിൽ(result)
        പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(alpn, "http/1.1")) {
      പൂർണ്ണസംഖ്യ httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_തിരുകുക_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      എങ്കിൽ(result)
        പോകുക out;
    }
#ifdef USE_NGHTTP2
    അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_തിരുകുക_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      എങ്കിൽ(result)
        പോകുക out;
    }
#endif /* USE_NGHTTP2 */
#എങ്കിൽ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_തിരുകുക_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      എങ്കിൽ(result)
        പോകുക out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    അല്ലെങ്കിൽ {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      പോകുക out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    പോകുക connect_sub;
  }
  അല്ലെങ്കിൽ {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  എങ്കിൽ(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_http_proxy_query(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_proxy_ctx *ctx = cf->ctx;
  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_HOST_PORT:
    *pres1 = (പൂർണ്ണസംഖ്യ)ctx->tunnel_peer->port;
    *((സ്ഥിരം അക്ഷരം **)pres2) = ctx->tunnel_peer->hostname;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_ALPN_NEGOTIATED: {
    സ്ഥിരം അക്ഷരം **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ശൂന്യ;
    തിരികെ CURLE_OK;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം ശൂന്യം cf_https_proxy_ctx_free(ഘടന cf_proxy_ctx *ctx)
{
  എങ്കിൽ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

സ്ഥിതം ശൂന്യം http_proxy_cf_destroy(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  ഘടന cf_proxy_ctx *ctx = cf->ctx;
  എങ്കിൽ(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

ഘടന Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                         ഘടന Curl_easy *data,
                                         ഘടന Curl_peer *peer,
                                         ഘടന Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  ഘടന Curl_cfilter *cf;
  ഘടന cf_proxy_ctx *ctx = ശൂന്യ;
  CURLcode result;

  (ശൂന്യം)data;
  എങ്കിൽ(!peer || !tunnel_peer)
    തിരികെ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  ctx = ശൂന്യ;
  Curl_conn_cf_തിരുകുക_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  തിരികെ result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  തിരഞ്ഞെടുപ്പ്(proxytype) {
  അവസ്ഥ CURLPROXY_HTTPS3:
    തിരികെ TRNSPRT_QUIC;
  സ്ഥിരസ്ഥിതി:
    തിരികെ TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
