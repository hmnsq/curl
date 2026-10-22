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

ساكن CURLcode dynhds_جمع_کړه_custom(بنية Curl_easy *معلومات,
                                  bool is_connect, صحيح httpversion,
                                  bool is_udp, بنية dynhds *hds)
{
  بنية connectdata *conn = معلومات->conn;
  بنية curl_slist *h[2];
  بنية curl_slist *سرلیکونه;
  صحيح numlists = 1; /* by default */
  صحيح i;

  تعداد Curl_proxy_use proxy;

  إذا(is_connect && !is_udp)
    proxy = سرلیک_CONNECT;
  وإلا إذا(is_connect && is_udp)
    proxy = سرلیک_CONNECT_UDP;
  وإلا
    proxy = conn->بټونه.origin_is_proxy ? سرلیک_PROXY : سرلیک_SERVER;

  بدل(proxy) {
  حالة سرلیک_SERVER:
    h[0] = معلومات->سېټ.سرلیکونه;
    اكسر;
  حالة سرلیک_PROXY:
    h[0] = معلومات->سېټ.سرلیکونه;
    إذا(معلومات->سېټ.sep_سرلیکونه) {
      h[1] = معلومات->سېټ.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة سرلیک_CONNECT:
    إذا(معلومات->سېټ.sep_سرلیکونه)
      h[0] = معلومات->سېټ.proxyheaders;
    وإلا
      h[0] = معلومات->سېټ.سرلیکونه;
    اكسر;
  حالة سرلیک_CONNECT_UDP:
    إذا(معلومات->سېټ.sep_سرلیکونه)
      h[0] = معلومات->سېټ.proxyheaders;
    وإلا
      h[0] = معلومات->سېټ.سرلیکونه;
    اكسر;
  }

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(سرلیکونه = h[i]; سرلیکونه; سرلیکونه = سرلیکونه->بل) {
      بنية Curl_str name;
      ثابت حرف *ارزښت = عدم;
      size_t valuelen = 0;
      ثابت حرف *ptr = سرلیکونه->معلومات;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      إذا(!curlx_str_cspn(&ptr, &name, ";:")) {
        إذا(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          إذا(*ptr) {
            ارزښت = ptr;
            valuelen = مزي_اوږدوالی(ارزښت);
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
            ارزښت = "";
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

      DEBUGASSERT(curlx_strlen(&name) && ارزښت);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      إذا(!curlx_strlen(&name))
        استمر;
      إذا(معلومات->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(معلومات->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(معلومات->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(معلومات->req.authneg &&
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
              !Curl_auth_allowed_منزل_host(معلومات))
        ;
      وإلا {
        CURLcode پایله =
          Curl_dynhds_جمع_کړه(hds, curlx_str(&name), curlx_strlen(&name),
                          ارزښت, valuelen);
        إذا(پایله)
          أرجع پایله;
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
  بټ(sub_filter_installed);
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
                                          بنية Curl_easy *معلومات,
                                          بنية Curl_peer *peer,
                                          بنية Curl_peer *dest,
                                          proxy_http_ver ver)
{
  حرف *authority = عدم;
  ثابت حرف *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  CURLcode پایله;
  بنية httpreq *req = عدم;

  (فراغ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->پورټ);
  إذا(!authority) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  پایله = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              عدم, 0, authority, مزي_اوږدوالی(authority),
                              عدم, 0);
  إذا(پایله)
    اذهب بهر;

  /* Setup the proxy-authorization header, if any */
  پایله = Curl_http_output_auth(معلومات, cf->conn, req->لاره, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(پایله)
    اذهب بهر;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(معلومات, cf->conn, STRCONST("Host"))) {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Host", authority);
    إذا(پایله)
      اذهب بهر;
  }

  إذا(معلومات->req.hd_proxy_auth) {
    پایله = Curl_dynhds_h1_cadd_کرښه(&req->سرلیکونه,
                                      معلومات->req.hd_proxy_auth);
    إذا(پایله)
      اذهب بهر;
  }

  ua = CURL_EASY_STR(معلومات, STRING_USERAGENT);
  إذا(!Curl_checkProxyheaders(معلومات, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "User-Agent", ua);
    إذا(پایله)
      اذهب بهر;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(معلومات, cf->conn, STRCONST("Proxy-Connection"))) {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Proxy-Connection", "Keep-Alive");
    إذا(پایله)
      اذهب بهر;
  }

  پایله = dynhds_جمع_کړه_custom(معلومات, TRUE, httpversion,
                             FALSE, &req->سرلیکونه);

بهر:
  إذا(پایله && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  *preq = req;
  أرجع پایله;
}

ساكن CURLcode http_proxy_create_CONNECTUDP(بنية httpreq **preq,
                                             بنية Curl_cfilter *cf,
                                             بنية Curl_easy *معلومات,
                                             بنية Curl_peer *peer,
                                             بنية Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ثابت حرف *proxy_scheme = "http", *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  حرف *authority = عدم;
  حرف *لار = عدم;
  حرف *encoded_host = عدم;
  بنية httpreq *req = عدم;
  CURLcode پایله;

  إذا(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->پورټ);
  إذا(!authority) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  إذا(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ثابت حرف *s = dest->hostname;
    حرف *d;
    size_t hlen = مزي_اوږدوالی(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    إذا(!encoded_host) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
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
    لار = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (بینښه صحيح)dest->پورټ);
  }
  وإلا {
    لار = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (بینښه صحيح)dest->پورټ);
  }

  إذا(!لار) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  إذا(ver == PROXY_HTTP_V1) {
    پایله = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, مزي_اوږدوالی(proxy_scheme),
                                authority, مزي_اوږدوالی(authority),
                                لار, مزي_اوږدوالی(لار));
    إذا(پایله)
      اذهب بهر;
  }
  وإلا إذا(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    پایله = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, مزي_اوږدوالی(proxy_scheme),
                                authority, مزي_اوږدوالی(authority),
                                لار, مزي_اوږدوالی(لار));
    إذا(پایله)
      اذهب بهر;
  }
  وإلا {
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }

  /* Setup the proxy-authorization header, if any */
  پایله = Curl_http_output_auth(معلومات, cf->conn, req->لاره, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(پایله)
    اذهب بهر;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(معلومات, cf->conn, STRCONST("Host"))) {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Host", authority);
    إذا(پایله)
      اذهب بهر;
  }

  إذا(معلومات->req.hd_proxy_auth) {
    پایله = Curl_dynhds_h1_cadd_کرښه(&req->سرلیکونه,
                                      معلومات->req.hd_proxy_auth);
    إذا(پایله)
      اذهب بهر;
  }

  ua = CURL_EASY_STR(معلومات, STRING_USERAGENT);
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(معلومات, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "User-Agent", ua);
    إذا(پایله)
      اذهب بهر;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(معلومات, cf->conn, STRCONST("Proxy-Connection"))) {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Proxy-Connection", "Keep-Alive");
    إذا(پایله)
      اذهب بهر;
  }

  إذا(ver == PROXY_HTTP_V1) {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Connection", "Upgrade");
    إذا(پایله)
      اذهب بهر;

    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Upgrade", "connect-udp");
    إذا(پایله)
      اذهب بهر;

    پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Capsule-Protocol", "?1");
    إذا(پایله)
      اذهب بهر;
  }
  وإلا {
    پایله = Curl_dynhds_cadd(&req->سرلیکونه, ":Protocol", "connect-udp");
    إذا(پایله)
      اذهب بهر;

    إذا(ver >= PROXY_HTTP_V2) {
      پایله = Curl_dynhds_cadd(&req->سرلیکونه, "Capsule-Protocol", "?1");
      إذا(پایله)
        اذهب بهر;
    }
  }

  پایله = dynhds_جمع_کړه_custom(معلومات, TRUE, httpversion,
                             TRUE, &req->سرلیکونه);

بهر:
  إذا(پایله && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  curlx_free(لار);
  curlx_free(encoded_host);
  *preq = req;
  أرجع پایله;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    بنية httpreq **preq, بنية Curl_cfilter *cf,
    بنية Curl_easy *معلومات, بنية Curl_peer *peer, بنية Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode پایله = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, معلومات, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, معلومات, peer, dest, ver);
  إذا(پایله)
    أرجع پایله;

  infof(معلومات, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->پورټ);
  أرجع CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
    بنية http_resp *resp, bool udp_tunnel,
    proxy_inspect_پایله *presult)
{
  بنية dynhds_ثبت *capsule_protocol = عدم;
  بنية dynhds_ثبت *auth_ځواب = عدم;
  size_t i, سرلیک_شمېر;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(resp);

  سرلیک_شمېر = Curl_dynhds_شمېر(&resp->سرلیکونه);
  infof(معلومات, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->حالت);
  infof(معلومات, "Response Headers (%zu total):", سرلیک_شمېر);
  لكل(i = 0; i < سرلیک_شمېر; i++) {
    بنية dynhds_ثبت *ثبت = Curl_dynhds_getn(&resp->سرلیکونه, i);
    إذا(ثبت)
      infof(معلومات, "  %s: %s", ثبت->name, ثبت->ارزښت);
  }

  إذا(resp->حالت == 407) {
    auth_ځواب = Curl_dynhds_cget(&resp->سرلیکونه, "Proxy-Authenticate");
  }

  إذا(auth_ځواب) {
    CURL_TRC_CF(معلومات, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_ځواب->ارزښت);
    پایله = Curl_http_ننوت_auth(معلومات, resp->حالت == 407,
                                  auth_ځواب->ارزښت);
    إذا(پایله)
      أرجع پایله;
    إذا(معلومات->req.newurl) {
      curlx_safefree(معلومات->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      أرجع CURLE_OK;
    }
  }

  إذا(udp_tunnel) {
    إذا(resp->حالت / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->سرلیکونه,
                                           "capsule-protocol");
      إذا(capsule_protocol) {
        إذا(!مزي_پرتله(capsule_protocol->ارزښت, "?1", 2) &&
           !capsule_protocol->ارزښت[2]) {
          infof(معلومات, "CONNECT-UDP tunnel established, response %d",
                resp->حالت);
          *presult = PROXY_INSPECT_OK;
          أرجع CURLE_OK;
        }
        failf(معلومات, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->حالت, capsule_protocol->ارزښت);
        *presult = PROXY_INSPECT_FAILED;
        أرجع CURLE_COULDNT_CONNECT;
      }
      وإلا {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(معلومات, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->حالت);
        *presult = PROXY_INSPECT_OK;
        أرجع CURLE_OK;
      }
    }
    وإلا {
      failf(معلومات, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->حالت);
      *presult = PROXY_INSPECT_FAILED;
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  إذا(resp->حالت / 100 == 2) {
    infof(معلومات, "CONNECT tunnel established, response %d", resp->حالت);
    *presult = PROXY_INSPECT_OK;
    أرجع CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  أرجع CURLE_COULDNT_CONNECT;
}

ساكن CURLcode http_proxy_cf_connect(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      bool *بشپړ)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(معلومات, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  إذا(cf->بل) {
    پایله = cf->بل->cft->do_connect(cf->بل, معلومات, بشپړ);
    إذا(پایله || !*بشپړ)
      أرجع پایله;
  }

  *بشپړ = FALSE;
  إذا(!ctx->sub_filter_installed) {
    ثابت حرف *alpn = عدم;

    /* in case of h3_proxy, cf->next will be NULL initially */
    إذا(cf->بل) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->بل, معلومات);
    }

    إذا(alpn)
      infof(معلومات, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    وإلا إذا(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(معلومات, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
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
      CURL_TRC_CF(معلومات, cf, "installing subfilter for HTTP/1.0");
      پایله = Curl_cf_h1_proxy_ورننباسه_after(cf, معلومات, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      إذا(پایله)
        اذهب بهر;
    }
    وإلا إذا(!strcmp(alpn, "http/1.1")) {
      صحيح httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(معلومات, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      پایله = Curl_cf_h1_proxy_ورننباسه_after(cf, معلومات, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      إذا(پایله)
        اذهب بهر;
    }
#که_تعریف_شوی USE_NGHTTP2
    وإلا إذا(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(معلومات, cf, "installing subfilter for HTTP/2");
      پایله = Curl_cf_h2_proxy_ورننباسه_after(cf, معلومات, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      إذا(پایله)
        اذهب بهر;
    }
#د_که_پای /* USE_NGHTTP2 */
#إذا defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    وإلا إذا(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(معلومات, cf, "installing subfilter for HTTP/3");
      پایله = Curl_cf_h3_proxy_ورننباسه_after(cf, معلومات, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      إذا(پایله)
        اذهب بهر;
    }
#د_که_پای /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    وإلا {
      failf(معلومات, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      پایله = CURLE_COULDNT_CONNECT;
      اذهب بهر;
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
    پایله = CURLE_OK;
  }

بهر:
  إذا(!پایله) {
    cf->connected = TRUE;
    *بشپړ = TRUE;
  }
  أرجع پایله;
}

ساكن CURLcode cf_http_proxy_query(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  بدل(query) {
  حالة CF_QUERY_HOST_پورټ:
    *pres1 = (صحيح)ctx->tunnel_peer->پورټ;
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
  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
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
                                  بنية Curl_easy *معلومات)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  إذا(ctx) {
    CURL_TRC_CF(معلومات, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

بنية Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_تیاری,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                         بنية Curl_easy *معلومات,
                                         بنية Curl_peer *peer,
                                         بنية Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  بنية Curl_cfilter *cf;
  بنية cf_proxy_ctx *ctx = عدم;
  CURLcode پایله;

  (فراغ)معلومات;
  إذا(!peer || !tunnel_peer)
    أرجع CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  پایله = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  إذا(پایله)
    اذهب بهر;
  ctx = عدم;
  Curl_conn_cf_ورننباسه_after(cf_at, cf);

بهر:
  cf_https_proxy_ctx_free(ctx);
  أرجع پایله;
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

#د_که_پای /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
