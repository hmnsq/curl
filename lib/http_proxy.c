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

স্থির CURLcode dynhds_add_custom(কাঠামো Curl_easy *তথ্য,
                                  bool is_connect, পূর্ণসংখ্যা httpversion,
                                  bool is_udp, কাঠামো dynhds *hds)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো curl_slist *h[2];
  কাঠামো curl_slist *headers;
  পূর্ণসংখ্যা numlists = 1; /* by default */
  পূর্ণসংখ্যা i;

  গণনা Curl_proxy_use proxy;

  যদি(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  অন্যথা যদি(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  অন্যথা
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  নির্বাচন(proxy) {
  ক্ষেত্র HEADER_SERVER:
    h[0] = তথ্য->set.headers;
    বিরতি;
  ক্ষেত্র HEADER_PROXY:
    h[0] = তথ্য->set.headers;
    যদি(তথ্য->set.sep_headers) {
      h[1] = তথ্য->set.proxyheaders;
      numlists++;
    }
    বিরতি;
  ক্ষেত্র HEADER_CONNECT:
    যদি(তথ্য->set.sep_headers)
      h[0] = তথ্য->set.proxyheaders;
    অন্যথা
      h[0] = তথ্য->set.headers;
    বিরতি;
  ক্ষেত্র HEADER_CONNECT_UDP:
    যদি(তথ্য->set.sep_headers)
      h[0] = তথ্য->set.proxyheaders;
    অন্যথা
      h[0] = তথ্য->set.headers;
    বিরতি;
  }

  /* loop through one or two lists */
  জন্য(i = 0; i < numlists; i++) {
    জন্য(headers = h[i]; headers; headers = headers->পরবর্তী) {
      কাঠামো Curl_str name;
      ধ্রুবক অক্ষর *মান = ফাঁকা;
      size_t valuelen = 0;
      ধ্রুবক অক্ষর *ptr = headers->তথ্য;

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
            চালু;
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
            চালু;
          }
        }
        অন্যথা
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          চালু;
      }
      অন্যথা
        /* no name, move on */
        চালু;

      DEBUGASSERT(curlx_strlen(&name) && মান);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      যদি(!curlx_strlen(&name))
        চালু;
      যদি(তথ্য->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      অন্যথা যদি(তথ্য->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(তথ্য->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      অন্যথা যদি(তথ্য->req.authneg &&
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
              !Curl_auth_allowed_to_host(তথ্য))
        ;
      অন্যথা {
        CURLcode ফলাফল =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          মান, valuelen);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
    }
  }

  ফেরত CURLE_OK;
}

কাঠামো cf_proxy_ctx {
  কাঠামো Curl_peer *peer; /* proxy */
  কাঠামো Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

স্থির পূর্ণসংখ্যা proxy_http_ver_major(proxy_http_ver ver)
{
  নির্বাচন(ver) {
  ক্ষেত্র PROXY_HTTP_V1:
    ফেরত 11;
  ক্ষেত্র PROXY_HTTP_V2:
    ফেরত 20;
  ক্ষেত্র PROXY_HTTP_V3:
    ফেরত 30;
  }
  ফেরত 0;
}

স্থির CURLcode http_proxy_create_CONNECT(কাঠামো httpreq **preq,
                                          কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য,
                                          কাঠামো Curl_peer *peer,
                                          কাঠামো Curl_peer *dest,
                                          proxy_http_ver ver)
{
  অক্ষর *authority = ফাঁকা;
  ধ্রুবক অক্ষর *ua;
  পূর্ণসংখ্যা httpversion = proxy_http_ver_major(ver);
  CURLcode ফলাফল;
  কাঠামো httpreq *req = ফাঁকা;

  (শূন্য)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  যদি(!authority) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  ফলাফল = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ফাঁকা, 0, authority, strlen(authority),
                              ফাঁকা, 0);
  যদি(ফলাফল)
    যাও out;

  /* Setup the proxy-authorization header, if any */
  ফলাফল = Curl_http_output_auth(তথ্য, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ফাঁকা, TRUE);
  যদি(ফলাফল)
    যাও out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(তথ্য, cf->conn, STRCONST("Host"))) {
    ফলাফল = Curl_dynhds_cadd(&req->headers, "Host", authority);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(তথ্য->req.hd_proxy_auth) {
    ফলাফল = Curl_dynhds_h1_cadd_সারি(&req->headers,
                                      তথ্য->req.hd_proxy_auth);
    যদি(ফলাফল)
      যাও out;
  }

  ua = CURL_EASY_STR(তথ্য, STRING_USERAGENT);
  যদি(!Curl_checkProxyheaders(তথ্য, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ফলাফল = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(তথ্য, cf->conn, STRCONST("Proxy-Connection"))) {
    ফলাফল = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    যদি(ফলাফল)
      যাও out;
  }

  ফলাফল = dynhds_add_custom(তথ্য, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  যদি(ফলাফল && req) {
    Curl_http_req_free(req);
    req = ফাঁকা;
  }
  curlx_free(authority);
  *preq = req;
  ফেরত ফলাফল;
}

স্থির CURLcode http_proxy_create_CONNECTUDP(কাঠামো httpreq **preq,
                                             কাঠামো Curl_cfilter *cf,
                                             কাঠামো Curl_easy *তথ্য,
                                             কাঠামো Curl_peer *peer,
                                             কাঠামো Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ধ্রুবক অক্ষর *proxy_scheme = "http", *ua;
  পূর্ণসংখ্যা httpversion = proxy_http_ver_major(ver);
  অক্ষর *authority = ফাঁকা;
  অক্ষর *path = ফাঁকা;
  অক্ষর *encoded_host = ফাঁকা;
  কাঠামো httpreq *req = ফাঁকা;
  CURLcode ফলাফল;

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
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  যদি(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ধ্রুবক অক্ষর *s = dest->hostname;
    অক্ষর *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    যদি(!encoded_host) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
    d = encoded_host;
    যখন(*s) {
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
                         encoded_host, (unsigned পূর্ণসংখ্যা)dest->port);
  }
  অন্যথা {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned পূর্ণসংখ্যা)dest->port);
  }

  যদি(!path) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  যদি(ver == PROXY_HTTP_V1) {
    ফলাফল = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা যদি(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    ফলাফল = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা {
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }

  /* Setup the proxy-authorization header, if any */
  ফলাফল = Curl_http_output_auth(তথ্য, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ফাঁকা, TRUE);
  যদি(ফলাফল)
    যাও out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(তথ্য, cf->conn, STRCONST("Host"))) {
    ফলাফল = Curl_dynhds_cadd(&req->headers, "Host", authority);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(তথ্য->req.hd_proxy_auth) {
    ফলাফল = Curl_dynhds_h1_cadd_সারি(&req->headers,
                                      তথ্য->req.hd_proxy_auth);
    যদি(ফলাফল)
      যাও out;
  }

  ua = CURL_EASY_STR(তথ্য, STRING_USERAGENT);
  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(তথ্য, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ফলাফল = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(তথ্য, cf->conn, STRCONST("Proxy-Connection"))) {
    ফলাফল = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    যদি(ফলাফল)
      যাও out;
  }

  যদি(ver == PROXY_HTTP_V1) {
    ফলাফল = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    যদি(ফলাফল)
      যাও out;

    ফলাফল = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    যদি(ফলাফল)
      যাও out;

    ফলাফল = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা {
    ফলাফল = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    যদি(ফলাফল)
      যাও out;

    যদি(ver >= PROXY_HTTP_V2) {
      ফলাফল = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      যদি(ফলাফল)
        যাও out;
    }
  }

  ফলাফল = dynhds_add_custom(তথ্য, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  যদি(ফলাফল && req) {
    Curl_http_req_free(req);
    req = ফাঁকা;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  ফেরত ফলাফল;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    কাঠামো httpreq **preq, কাঠামো Curl_cfilter *cf,
    কাঠামো Curl_easy *তথ্য, কাঠামো Curl_peer *peer, কাঠামো Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode ফলাফল = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, তথ্য, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, তথ্য, peer, dest, ver);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  infof(তথ্য, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  ফেরত CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
    কাঠামো http_resp *resp, bool udp_tunnel,
    proxy_inspect_ফলাফল *presult)
{
  কাঠামো dynhds_entry *capsule_protocol = ফাঁকা;
  কাঠামো dynhds_entry *auth_reply = ফাঁকা;
  size_t i, header_গুনতি;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(resp);

  header_গুনতি = Curl_dynhds_গুনতি(&resp->headers);
  infof(তথ্য, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(তথ্য, "Response Headers (%zu total):", header_গুনতি);
  জন্য(i = 0; i < header_গুনতি; i++) {
    কাঠামো dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    যদি(entry)
      infof(তথ্য, "  %s: %s", entry->name, entry->মান);
  }

  যদি(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  যদি(auth_reply) {
    CURL_TRC_CF(তথ্য, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->মান);
    ফলাফল = Curl_http_প্রবেশ_auth(তথ্য, resp->status == 407,
                                  auth_reply->মান);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(তথ্য->req.newurl) {
      curlx_safefree(তথ্য->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      ফেরত CURLE_OK;
    }
  }

  যদি(udp_tunnel) {
    যদি(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      যদি(capsule_protocol) {
        যদি(!strncmp(capsule_protocol->মান, "?1", 2) &&
           !capsule_protocol->মান[2]) {
          infof(তথ্য, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          ফেরত CURLE_OK;
        }
        failf(তথ্য, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->মান);
        *presult = PROXY_INSPECT_FAILED;
        ফেরত CURLE_COULDNT_CONNECT;
      }
      অন্যথা {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(তথ্য, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        ফেরত CURLE_OK;
      }
    }
    অন্যথা {
      failf(তথ্য, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      ফেরত CURLE_COULDNT_CONNECT;
    }
  }

  যদি(resp->status / 100 == 2) {
    infof(তথ্য, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    ফেরত CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  ফেরত CURLE_COULDNT_CONNECT;
}

স্থির CURLcode http_proxy_cf_connect(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      bool *done)
{
  কাঠামো cf_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  CURL_TRC_CF(তথ্য, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  যদি(cf->পরবর্তী) {
    ফলাফল = cf->পরবর্তী->cft->do_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  *done = FALSE;
  যদি(!ctx->sub_filter_installed) {
    ধ্রুবক অক্ষর *alpn = ফাঁকা;

    /* in case of h3_proxy, cf->next will be NULL initially */
    যদি(cf->পরবর্তী) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->পরবর্তী, তথ্য);
    }

    যদি(alpn)
      infof(তথ্য, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    অন্যথা যদি(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(তথ্য, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      নির্বাচন(ctx->proxytype) {
      ক্ষেত্র CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        বিরতি;
      ক্ষেত্র CURLPROXY_HTTPS2:
        alpn = "h2";
        বিরতি;
      ক্ষেত্র CURLPROXY_HTTPS3:
        alpn = "h3";
        বিরতি;
      পূর্বনির্ধারিত:
        alpn = "http/1.1";
        বিরতি;
      }
    }

    যদি(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(তথ্য, cf, "installing subfilter for HTTP/1.0");
      ফলাফল = Curl_cf_h1_proxy_ঢোকাও_after(cf, তথ্য, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      যদি(ফলাফল)
        যাও out;
    }
    অন্যথা যদি(!strcmp(alpn, "http/1.1")) {
      পূর্ণসংখ্যা httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(তথ্য, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      ফলাফল = Curl_cf_h1_proxy_ঢোকাও_after(cf, তথ্য, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      যদি(ফলাফল)
        যাও out;
    }
#ifdef USE_NGHTTP2
    অন্যথা যদি(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(তথ্য, cf, "installing subfilter for HTTP/2");
      ফলাফল = Curl_cf_h2_proxy_ঢোকাও_after(cf, তথ্য, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      যদি(ফলাফল)
        যাও out;
    }
#endif /* USE_NGHTTP2 */
#যদি defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    অন্যথা যদি(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(তথ্য, cf, "installing subfilter for HTTP/3");
      ফলাফল = Curl_cf_h3_proxy_ঢোকাও_after(cf, তথ্য, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      যদি(ফলাফল)
        যাও out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    অন্যথা {
      failf(তথ্য, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      ফলাফল = CURLE_COULDNT_CONNECT;
      যাও out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    যাও connect_sub;
  }
  অন্যথা {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    ফলাফল = CURLE_OK;
  }

out:
  যদি(!ফলাফল) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_http_proxy_query(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_proxy_ctx *ctx = cf->ctx;
  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_HOST_PORT:
    *pres1 = (পূর্ণসংখ্যা)ctx->tunnel_peer->port;
    *((ধ্রুবক অক্ষর **)pres2) = ctx->tunnel_peer->hostname;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুবক অক্ষর **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ফাঁকা;
    ফেরত CURLE_OK;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির শূন্য cf_https_proxy_ctx_free(কাঠামো cf_proxy_ctx *ctx)
{
  যদি(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

স্থির শূন্য http_proxy_cf_destroy(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_proxy_ctx *ctx = cf->ctx;
  যদি(ctx) {
    CURL_TRC_CF(তথ্য, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

কাঠামো Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SETUP,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো Curl_peer *peer,
                                         কাঠামো Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  কাঠামো Curl_cfilter *cf;
  কাঠামো cf_proxy_ctx *ctx = ফাঁকা;
  CURLcode ফলাফল;

  (শূন্য)তথ্য;
  যদি(!peer || !tunnel_peer)
    ফেরত CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  যদি(ফলাফল)
    যাও out;
  ctx = ফাঁকা;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  ফেরত ফলাফল;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  নির্বাচন(proxytype) {
  ক্ষেত্র CURLPROXY_HTTPS3:
    ফেরত TRNSPRT_QUIC;
  পূর্বনির্ধারিত:
    ফেরত TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
