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

ساكن CURLcode dynhds_جمع_کن_custom(بنية Curl_easy *داده,
                                  bool is_connect, صحيح httpversion,
                                  bool is_udp, بنية dynhds *hds)
{
  بنية connectdata *conn = داده->conn;
  بنية curl_slist *h[2];
  بنية curl_slist *سرآیندها;
  صحيح numlists = 1; /* by default */
  صحيح i;

  تعداد Curl_proxy_use proxy;

  إذا(is_connect && !is_udp)
    proxy = سرآیند_CONNECT;
  وإلا إذا(is_connect && is_udp)
    proxy = سرآیند_CONNECT_UDP;
  وإلا
    proxy = conn->بیتها.origin_is_proxy ? سرآیند_PROXY : سرآیند_SERVER;

  بدل(proxy) {
  حالة سرآیند_SERVER:
    h[0] = داده->مجموعه.سرآیندها;
    اكسر;
  حالة سرآیند_PROXY:
    h[0] = داده->مجموعه.سرآیندها;
    إذا(داده->مجموعه.sep_سرآیندها) {
      h[1] = داده->مجموعه.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة سرآیند_CONNECT:
    إذا(داده->مجموعه.sep_سرآیندها)
      h[0] = داده->مجموعه.proxyheaders;
    وإلا
      h[0] = داده->مجموعه.سرآیندها;
    اكسر;
  حالة سرآیند_CONNECT_UDP:
    إذا(داده->مجموعه.sep_سرآیندها)
      h[0] = داده->مجموعه.proxyheaders;
    وإلا
      h[0] = داده->مجموعه.سرآیندها;
    اكسر;
  }

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(سرآیندها = h[i]; سرآیندها; سرآیندها = سرآیندها->بعدی) {
      بنية Curl_str name;
      ثابت حرف *مقدار = عدم;
      size_t valuelen = 0;
      ثابت حرف *ptr = سرآیندها->داده;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      إذا(!curlx_str_cspn(&ptr, &name, ";:")) {
        إذا(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          إذا(*ptr) {
            مقدار = ptr;
            valuelen = طول_رشته(مقدار);
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
            مقدار = "";
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

      DEBUGASSERT(curlx_strlen(&name) && مقدار);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      إذا(!curlx_strlen(&name))
        استمر;
      إذا(داده->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(داده->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(داده->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(داده->req.authneg &&
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
              !Curl_auth_allowed_مقصد_host(داده))
        ;
      وإلا {
        CURLcode نتیجه =
          Curl_dynhds_جمع_کن(hds, curlx_str(&name), curlx_strlen(&name),
                          مقدار, valuelen);
        إذا(نتیجه)
          أرجع نتیجه;
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
  بیت(sub_filter_installed);
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
                                          بنية Curl_easy *داده,
                                          بنية Curl_peer *peer,
                                          بنية Curl_peer *dest,
                                          proxy_http_ver ver)
{
  حرف *authority = عدم;
  ثابت حرف *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  CURLcode نتیجه;
  بنية httpreq *req = عدم;

  (فراغ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->پورت);
  إذا(!authority) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  نتیجه = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              عدم, 0, authority, طول_رشته(authority),
                              عدم, 0);
  إذا(نتیجه)
    اذهب بیرون;

  /* Setup the proxy-authorization header, if any */
  نتیجه = Curl_http_output_auth(داده, cf->conn, req->روش, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(نتیجه)
    اذهب بیرون;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(داده, cf->conn, STRCONST("Host"))) {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Host", authority);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(داده->req.hd_proxy_auth) {
    نتیجه = Curl_dynhds_h1_cadd_سطر(&req->سرآیندها,
                                      داده->req.hd_proxy_auth);
    إذا(نتیجه)
      اذهب بیرون;
  }

  ua = CURL_EASY_STR(داده, STRING_USERAGENT);
  إذا(!Curl_checkProxyheaders(داده, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "User-Agent", ua);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(داده, cf->conn, STRCONST("Proxy-Connection"))) {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Proxy-Connection", "Keep-Alive");
    إذا(نتیجه)
      اذهب بیرون;
  }

  نتیجه = dynhds_جمع_کن_custom(داده, TRUE, httpversion,
                             FALSE, &req->سرآیندها);

بیرون:
  إذا(نتیجه && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  *preq = req;
  أرجع نتیجه;
}

ساكن CURLcode http_proxy_create_CONNECTUDP(بنية httpreq **preq,
                                             بنية Curl_cfilter *cf,
                                             بنية Curl_easy *داده,
                                             بنية Curl_peer *peer,
                                             بنية Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ثابت حرف *proxy_scheme = "http", *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  حرف *authority = عدم;
  حرف *مسیر = عدم;
  حرف *encoded_host = عدم;
  بنية httpreq *req = عدم;
  CURLcode نتیجه;

  إذا(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->پورت);
  إذا(!authority) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  إذا(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ثابت حرف *s = dest->hostname;
    حرف *d;
    size_t hlen = طول_رشته(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    إذا(!encoded_host) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
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
    مسیر = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (بیعلامت صحيح)dest->پورت);
  }
  وإلا {
    مسیر = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (بیعلامت صحيح)dest->پورت);
  }

  إذا(!مسیر) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  إذا(ver == PROXY_HTTP_V1) {
    نتیجه = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, طول_رشته(proxy_scheme),
                                authority, طول_رشته(authority),
                                مسیر, طول_رشته(مسیر));
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا إذا(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    نتیجه = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, طول_رشته(proxy_scheme),
                                authority, طول_رشته(authority),
                                مسیر, طول_رشته(مسیر));
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا {
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }

  /* Setup the proxy-authorization header, if any */
  نتیجه = Curl_http_output_auth(داده, cf->conn, req->روش, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(نتیجه)
    اذهب بیرون;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(داده, cf->conn, STRCONST("Host"))) {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Host", authority);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(داده->req.hd_proxy_auth) {
    نتیجه = Curl_dynhds_h1_cadd_سطر(&req->سرآیندها,
                                      داده->req.hd_proxy_auth);
    إذا(نتیجه)
      اذهب بیرون;
  }

  ua = CURL_EASY_STR(داده, STRING_USERAGENT);
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(داده, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "User-Agent", ua);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(داده, cf->conn, STRCONST("Proxy-Connection"))) {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Proxy-Connection", "Keep-Alive");
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(ver == PROXY_HTTP_V1) {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Connection", "Upgrade");
    إذا(نتیجه)
      اذهب بیرون;

    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Upgrade", "connect-udp");
    إذا(نتیجه)
      اذهب بیرون;

    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Capsule-Protocol", "?1");
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا {
    نتیجه = Curl_dynhds_cadd(&req->سرآیندها, ":Protocol", "connect-udp");
    إذا(نتیجه)
      اذهب بیرون;

    إذا(ver >= PROXY_HTTP_V2) {
      نتیجه = Curl_dynhds_cadd(&req->سرآیندها, "Capsule-Protocol", "?1");
      إذا(نتیجه)
        اذهب بیرون;
    }
  }

  نتیجه = dynhds_جمع_کن_custom(داده, TRUE, httpversion,
                             TRUE, &req->سرآیندها);

بیرون:
  إذا(نتیجه && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  curlx_free(مسیر);
  curlx_free(encoded_host);
  *preq = req;
  أرجع نتیجه;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    بنية httpreq **preq, بنية Curl_cfilter *cf,
    بنية Curl_easy *داده, بنية Curl_peer *peer, بنية Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode نتیجه = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, داده, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, داده, peer, dest, ver);
  إذا(نتیجه)
    أرجع نتیجه;

  infof(داده, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->پورت);
  أرجع CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    بنية Curl_cfilter *cf, بنية Curl_easy *داده,
    بنية http_resp *resp, bool udp_tunnel,
    proxy_inspect_نتیجه *presult)
{
  بنية dynhds_مدخل *capsule_protocol = عدم;
  بنية dynhds_مدخل *auth_پاسخ = عدم;
  size_t i, سرآیند_شمار;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(resp);

  سرآیند_شمار = Curl_dynhds_شمار(&resp->سرآیندها);
  infof(داده, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->وضعیت);
  infof(داده, "Response Headers (%zu total):", سرآیند_شمار);
  لكل(i = 0; i < سرآیند_شمار; i++) {
    بنية dynhds_مدخل *مدخل = Curl_dynhds_getn(&resp->سرآیندها, i);
    إذا(مدخل)
      infof(داده, "  %s: %s", مدخل->name, مدخل->مقدار);
  }

  إذا(resp->وضعیت == 407) {
    auth_پاسخ = Curl_dynhds_cget(&resp->سرآیندها, "Proxy-Authenticate");
  }

  إذا(auth_پاسخ) {
    CURL_TRC_CF(داده, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_پاسخ->مقدار);
    نتیجه = Curl_http_درون_داد_auth(داده, resp->وضعیت == 407,
                                  auth_پاسخ->مقدار);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(داده->req.newurl) {
      curlx_safefree(داده->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      أرجع CURLE_OK;
    }
  }

  إذا(udp_tunnel) {
    إذا(resp->وضعیت / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->سرآیندها,
                                           "capsule-protocol");
      إذا(capsule_protocol) {
        إذا(!مقایسه_رشته(capsule_protocol->مقدار, "?1", 2) &&
           !capsule_protocol->مقدار[2]) {
          infof(داده, "CONNECT-UDP tunnel established, response %d",
                resp->وضعیت);
          *presult = PROXY_INSPECT_OK;
          أرجع CURLE_OK;
        }
        failf(داده, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->وضعیت, capsule_protocol->مقدار);
        *presult = PROXY_INSPECT_FAILED;
        أرجع CURLE_COULDNT_CONNECT;
      }
      وإلا {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(داده, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->وضعیت);
        *presult = PROXY_INSPECT_OK;
        أرجع CURLE_OK;
      }
    }
    وإلا {
      failf(داده, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->وضعیت);
      *presult = PROXY_INSPECT_FAILED;
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  إذا(resp->وضعیت / 100 == 2) {
    infof(داده, "CONNECT tunnel established, response %d", resp->وضعیت);
    *presult = PROXY_INSPECT_OK;
    أرجع CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  أرجع CURLE_COULDNT_CONNECT;
}

ساكن CURLcode http_proxy_cf_connect(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      bool *انجامشده)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(داده, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  إذا(cf->بعدی) {
    نتیجه = cf->بعدی->cft->do_connect(cf->بعدی, داده, انجامشده);
    إذا(نتیجه || !*انجامشده)
      أرجع نتیجه;
  }

  *انجامشده = FALSE;
  إذا(!ctx->sub_filter_installed) {
    ثابت حرف *alpn = عدم;

    /* in case of h3_proxy, cf->next will be NULL initially */
    إذا(cf->بعدی) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->بعدی, داده);
    }

    إذا(alpn)
      infof(داده, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    وإلا إذا(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(داده, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
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
      CURL_TRC_CF(داده, cf, "installing subfilter for HTTP/1.0");
      نتیجه = Curl_cf_h1_proxy_درج_after(cf, داده, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      إذا(نتیجه)
        اذهب بیرون;
    }
    وإلا إذا(!strcmp(alpn, "http/1.1")) {
      صحيح httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(داده, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      نتیجه = Curl_cf_h1_proxy_درج_after(cf, داده, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      إذا(نتیجه)
        اذهب بیرون;
    }
#اگر_تعریف_شده USE_NGHTTP2
    وإلا إذا(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(داده, cf, "installing subfilter for HTTP/2");
      نتیجه = Curl_cf_h2_proxy_درج_after(cf, داده, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      إذا(نتیجه)
        اذهب بیرون;
    }
#پایان_اگر /* USE_NGHTTP2 */
#إذا defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    وإلا إذا(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(داده, cf, "installing subfilter for HTTP/3");
      نتیجه = Curl_cf_h3_proxy_درج_after(cf, داده, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      إذا(نتیجه)
        اذهب بیرون;
    }
#پایان_اگر /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    وإلا {
      failf(داده, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      نتیجه = CURLE_COULDNT_CONNECT;
      اذهب بیرون;
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
    نتیجه = CURLE_OK;
  }

بیرون:
  إذا(!نتیجه) {
    cf->connected = TRUE;
    *انجامشده = TRUE;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_http_proxy_query(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  بدل(query) {
  حالة CF_QUERY_HOST_پورت:
    *pres1 = (صحيح)ctx->tunnel_peer->پورت;
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
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
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
                                  بنية Curl_easy *داده)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  إذا(ctx) {
    CURL_TRC_CF(داده, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

بنية Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_راهاندازی,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_داده_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_درج_after(بنية Curl_cfilter *cf_at,
                                         بنية Curl_easy *داده,
                                         بنية Curl_peer *peer,
                                         بنية Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  بنية Curl_cfilter *cf;
  بنية cf_proxy_ctx *ctx = عدم;
  CURLcode نتیجه;

  (فراغ)داده;
  إذا(!peer || !tunnel_peer)
    أرجع CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  ctx = عدم;
  Curl_conn_cf_درج_after(cf_at, cf);

بیرون:
  cf_https_proxy_ctx_free(ctx);
  أرجع نتیجه;
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

#پایان_اگر /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
