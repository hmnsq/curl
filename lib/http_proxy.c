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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "http_proxy.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-h1-proxy.h"
#ଅନ୍ତର୍ଭାଵ "cf-h2-proxy.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

ସ୍ଥିର CURLcode dynhds_add_custom(ସଂରଚନା Curl_easy *data,
                                  bool is_connect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion,
                                  bool is_udp, ସଂରଚନା dynhds *hds)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା curl_slist *h[2];
  ସଂରଚନା curl_slist *headers;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା numlists = 1; /* by default */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଗଣନା Curl_proxy_use proxy;

  ଯଦି(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  ଅନ୍ୟଥା ଯଦି(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  ଅନ୍ୟଥା
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  ବାଛ(proxy) {
  ସ୍ଥିତି HEADER_SERVER:
    h[0] = data->set.headers;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HEADER_PROXY:
    h[0] = data->set.headers;
    ଯଦି(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HEADER_CONNECT:
    ଯଦି(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ଅନ୍ୟଥା
      h[0] = data->set.headers;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି HEADER_CONNECT_UDP:
    ଯଦି(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ଅନ୍ୟଥା
      h[0] = data->set.headers;
    ଭାଙ୍ଗ;
  }

  /* loop through one or two lists */
  ପାଇଁ(i = 0; i < numlists; i++) {
    ପାଇଁ(headers = h[i]; headers; headers = headers->next) {
      ସଂରଚନା Curl_str name;
      ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
      size_t valuelen = 0;
      ଅଚଳ ଅକ୍ଷର *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ଯଦି(!curlx_str_cspn(&ptr, &name, ";:")) {
        ଯଦି(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ଯଦି(*ptr) {
            ମୂଲ୍ୟ = ptr;
            valuelen = strlen(ମୂଲ୍ୟ);
          }
          ଅନ୍ୟଥା {
            /* quirk #1, suppress this header */
            ଜାରି;
          }
        }
        ଅନ୍ୟଥା ଯଦି(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ଯଦି(!*ptr) {
            /* quirk #2, send an empty header */
            ମୂଲ୍ୟ = "";
            valuelen = 0;
          }
          ଅନ୍ୟଥା {
            /* this may be used for something else in the future,
             * ignore this for now */
            ଜାରି;
          }
        }
        ଅନ୍ୟଥା
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          ଜାରି;
      }
      ଅନ୍ୟଥା
        /* no name, move on */
        ଜାରି;

      DEBUGASSERT(curlx_strlen(&name) && ମୂଲ୍ୟ);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ଯଦି(!curlx_strlen(&name))
        ଜାରି;
      ଯଦି(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ଅନ୍ୟଥା ଯଦି(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ଅନ୍ୟଥା ଯଦି(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ଅନ୍ୟଥା ଯଦି(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ଅନ୍ୟଥା ଯଦି((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      ଅନ୍ୟଥା ଯଦି((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      ଅନ୍ୟଥା {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          ମୂଲ୍ୟ, valuelen);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା cf_proxy_ctx {
  ସଂରଚନା Curl_peer *peer; /* proxy */
  ସଂରଚନା Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_http_ver_major(proxy_http_ver ver)
{
  ବାଛ(ver) {
  ସ୍ଥିତି PROXY_HTTP_V1:
    ଫେରନ୍ତୁ 11;
  ସ୍ଥିତି PROXY_HTTP_V2:
    ଫେରନ୍ତୁ 20;
  ସ୍ଥିତି PROXY_HTTP_V3:
    ଫେରନ୍ତୁ 30;
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର CURLcode http_proxy_create_CONNECT(ସଂରଚନା httpreq **preq,
                                          ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା Curl_peer *peer,
                                          ସଂରଚନା Curl_peer *dest,
                                          proxy_http_ver ver)
{
  ଅକ୍ଷର *authority = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *ua;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  ସଂରଚନା httpreq *req = ଖାଲି;

  (ଶୂନ୍ୟ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  ଯଦି(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ଖାଲି, 0, authority, strlen(authority),
                              ଖାଲି, 0);
  ଯଦି(result)
    ଯାଅ out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ଖାଲି, TRUE);
  ଯଦି(result)
    ଯାଅ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ଯଦି(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_ଧାଡ଼ି(&req->headers,
                                      data->req.hd_proxy_auth);
    ଯଦି(result)
      ଯାଅ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ଯଦି(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ଯଦି(result)
      ଯାଅ out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  ଯଦି(result && req) {
    Curl_http_req_free(req);
    req = ଖାଲି;
  }
  curlx_free(authority);
  *preq = req;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode http_proxy_create_CONNECTUDP(ସଂରଚନା httpreq **preq,
                                             ସଂରଚନା Curl_cfilter *cf,
                                             ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା Curl_peer *peer,
                                             ସଂରଚନା Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ଅଚଳ ଅକ୍ଷର *proxy_scheme = "http", *ua;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion = proxy_http_ver_major(ver);
  ଅକ୍ଷର *authority = ଖାଲି;
  ଅକ୍ଷର *path = ଖାଲି;
  ଅକ୍ଷର *encoded_host = ଖାଲି;
  ସଂରଚନା httpreq *req = ଖାଲି;
  CURLcode result;

  ଯଦି(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  ଯଦି(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ଯଦି(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ଅଚଳ ଅକ୍ଷର *s = dest->hostname;
    ଅକ୍ଷର *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ଯଦି(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
    d = encoded_host;
    ଯେତେବେଳେ(*s) {
      ଯଦି(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      ଅନ୍ୟଥା
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dest->port);
  }
  ଅନ୍ୟଥା {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dest->port);
  }

  ଯଦି(!path) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ଯଦି(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା {
    result = CURLE_FAILED_INIT;
    ଯାଅ out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ଖାଲି, TRUE);
  ଯଦି(result)
    ଯାଅ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ଯଦି(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_ଧାଡ଼ି(&req->headers,
                                      data->req.hd_proxy_auth);
    ଯଦି(result)
      ଯାଅ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ଯଦି(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    ଯଦି(result)
      ଯାଅ out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    ଯଦି(result)
      ଯାଅ out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    ଯଦି(result)
      ଯାଅ out;

    ଯଦି(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      ଯଦି(result)
        ଯାଅ out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  ଯଦି(result && req) {
    Curl_http_req_free(req);
    req = ଖାଲି;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    ସଂରଚନା httpreq **preq, ସଂରଚନା Curl_cfilter *cf,
    ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_peer *peer, ସଂରଚନା Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
    ସଂରଚନା http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  ସଂରଚନା dynhds_entry *capsule_protocol = ଖାଲି;
  ସଂରଚନା dynhds_entry *auth_reply = ଖାଲି;
  size_t i, header_ଗଣନା;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_ଗଣନା = Curl_dynhds_ଗଣନା(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_ଗଣନା);
  ପାଇଁ(i = 0; i < header_ଗଣନା; i++) {
    ସଂରଚନା dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    ଯଦି(entry)
      infof(data, "  %s: %s", entry->name, entry->ମୂଲ୍ୟ);
  }

  ଯଦି(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  ଯଦି(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->ମୂଲ୍ୟ);
    result = Curl_http_ନିବେଶ_auth(data, resp->status == 407,
                                  auth_reply->ମୂଲ୍ୟ);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      ଫେରନ୍ତୁ CURLE_OK;
    }
  }

  ଯଦି(udp_tunnel) {
    ଯଦି(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      ଯଦି(capsule_protocol) {
        ଯଦି(!strncmp(capsule_protocol->ମୂଲ୍ୟ, "?1", 2) &&
           !capsule_protocol->ମୂଲ୍ୟ[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          ଫେରନ୍ତୁ CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->ମୂଲ୍ୟ);
        *presult = PROXY_INSPECT_FAILED;
        ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
      }
      ଅନ୍ୟଥା {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        ଫେରନ୍ତୁ CURLE_OK;
      }
    }
    ଅନ୍ୟଥା {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }
  }

  ଯଦି(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
}

ସ୍ଥିର CURLcode http_proxy_cf_connect(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      bool *done)
{
  ସଂରଚନା cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ଯଦି(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  *done = FALSE;
  ଯଦି(!ctx->sub_filter_installed) {
    ଅଚଳ ଅକ୍ଷର *alpn = ଖାଲି;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ଯଦି(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    ଯଦି(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    ଅନ୍ୟଥା ଯଦି(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      ବାଛ(ctx->proxytype) {
      ସ୍ଥିତି CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURLPROXY_HTTPS2:
        alpn = "h2";
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURLPROXY_HTTPS3:
        alpn = "h3";
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        alpn = "http/1.1";
        ଭାଙ୍ଗ;
      }
    }

    ଯଦି(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_ଭର୍ତ୍ତି_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ଯଦି(result)
        ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(!strcmp(alpn, "http/1.1")) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_ଭର୍ତ୍ତି_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ଯଦି(result)
        ଯାଅ out;
    }
#ifdef USE_NGHTTP2
    ଅନ୍ୟଥା ଯଦି(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_ଭର୍ତ୍ତି_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ଯଦି(result)
        ଯାଅ out;
    }
#endif /* USE_NGHTTP2 */
#ଯଦି defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    ଅନ୍ୟଥା ଯଦି(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_ଭର୍ତ୍ତି_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ଯଦି(result)
        ଯାଅ out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    ଅନ୍ୟଥା {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      ଯାଅ out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    ଯାଅ connect_sub;
  }
  ଅନ୍ୟଥା {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  ଯଦି(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_http_proxy_query(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_proxy_ctx *ctx = cf->ctx;
  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_HOST_PORT:
    *pres1 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ctx->tunnel_peer->port;
    *((ଅଚଳ ଅକ୍ଷର **)pres2) = ctx->tunnel_peer->hostname;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_ALPN_NEGOTIATED: {
    ଅଚଳ ଅକ୍ଷର **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_https_proxy_ctx_free(ସଂରଚନା cf_proxy_ctx *ctx)
{
  ଯଦି(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ http_proxy_cf_destroy(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_proxy_ctx *ctx = cf->ctx;
  ଯଦି(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

ସଂରଚନା Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                         ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା Curl_peer *peer,
                                         ସଂରଚନା Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା cf_proxy_ctx *ctx = ଖାଲି;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  ଯଦି(!peer || !tunnel_peer)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ଯଦି(result)
    ଯାଅ out;
  ctx = ଖାଲି;
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  ଫେରନ୍ତୁ result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  ବାଛ(proxytype) {
  ସ୍ଥିତି CURLPROXY_HTTPS3:
    ଫେରନ୍ତୁ TRNSPRT_QUIC;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
