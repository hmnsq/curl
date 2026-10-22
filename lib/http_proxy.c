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

ساكن CURLcode dynhds_جوڑو_custom(بنية Curl_easy *انکڑے,
                                  bool is_connect, صحيح httpversion,
                                  bool is_udp, بنية dynhds *hds)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية curl_slist *h[2];
  بنية curl_slist *سرخیاں;
  صحيح numlists = 1; /* by default */
  صحيح i;

  تعداد Curl_proxy_use proxy;

  إذا(is_connect && !is_udp)
    proxy = سرخی_CONNECT;
  وإلا إذا(is_connect && is_udp)
    proxy = سرخی_CONNECT_UDP;
  وإلا
    proxy = conn->بٹ.origin_is_proxy ? سرخی_PROXY : سرخی_SERVER;

  بدل(proxy) {
  حالة سرخی_SERVER:
    h[0] = انکڑے->سیٹ.سرخیاں;
    اكسر;
  حالة سرخی_PROXY:
    h[0] = انکڑے->سیٹ.سرخیاں;
    إذا(انکڑے->سیٹ.sep_سرخیاں) {
      h[1] = انکڑے->سیٹ.proxyheaders;
      numlists++;
    }
    اكسر;
  حالة سرخی_CONNECT:
    إذا(انکڑے->سیٹ.sep_سرخیاں)
      h[0] = انکڑے->سیٹ.proxyheaders;
    وإلا
      h[0] = انکڑے->سیٹ.سرخیاں;
    اكسر;
  حالة سرخی_CONNECT_UDP:
    إذا(انکڑے->سیٹ.sep_سرخیاں)
      h[0] = انکڑے->سیٹ.proxyheaders;
    وإلا
      h[0] = انکڑے->سیٹ.سرخیاں;
    اكسر;
  }

  /* loop through one or two lists */
  لكل(i = 0; i < numlists; i++) {
    لكل(سرخیاں = h[i]; سرخیاں; سرخیاں = سرخیاں->اگلا) {
      بنية Curl_str name;
      ثابت حرف *قدر = عدم;
      size_t valuelen = 0;
      ثابت حرف *ptr = سرخیاں->انکڑے;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      إذا(!curlx_str_cspn(&ptr, &name, ";:")) {
        إذا(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          إذا(*ptr) {
            قدر = ptr;
            valuelen = لڑی_لمبائی(قدر);
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
      إذا(انکڑے->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      وإلا إذا(انکڑے->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(انکڑے->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      وإلا إذا(انکڑے->req.authneg &&
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
              !Curl_auth_allowed_منزل_host(انکڑے))
        ;
      وإلا {
        CURLcode نتیجہ =
          Curl_dynhds_جوڑو(hds, curlx_str(&name), curlx_strlen(&name),
                          قدر, valuelen);
        إذا(نتیجہ)
          أرجع نتیجہ;
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
  ثنائی_ہندسہ(sub_filter_installed);
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
                                          بنية Curl_easy *انکڑے,
                                          بنية Curl_peer *peer,
                                          بنية Curl_peer *dest,
                                          proxy_http_ver ver)
{
  حرف *authority = عدم;
  ثابت حرف *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  CURLcode نتیجہ;
  بنية httpreq *req = عدم;

  (فراغ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->پورٹ);
  إذا(!authority) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  نتیجہ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              عدم, 0, authority, لڑی_لمبائی(authority),
                              عدم, 0);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* Setup the proxy-authorization header, if any */
  نتیجہ = Curl_http_output_auth(انکڑے, cf->conn, req->طریقہ, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(انکڑے, cf->conn, STRCONST("Host"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Host", authority);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(انکڑے->req.hd_proxy_auth) {
    نتیجہ = Curl_dynhds_h1_cadd_سطر(&req->سرخیاں,
                                      انکڑے->req.hd_proxy_auth);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  ua = CURL_EASY_STR(انکڑے, STRING_USERAGENT);
  إذا(!Curl_checkProxyheaders(انکڑے, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "User-Agent", ua);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(انکڑے, cf->conn, STRCONST("Proxy-Connection"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Proxy-Connection", "Keep-Alive");
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  نتیجہ = dynhds_جوڑو_custom(انکڑے, TRUE, httpversion,
                             FALSE, &req->سرخیاں);

باہرلا:
  إذا(نتیجہ && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  *preq = req;
  أرجع نتیجہ;
}

ساكن CURLcode http_proxy_create_CONNECTUDP(بنية httpreq **preq,
                                             بنية Curl_cfilter *cf,
                                             بنية Curl_easy *انکڑے,
                                             بنية Curl_peer *peer,
                                             بنية Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ثابت حرف *proxy_scheme = "http", *ua;
  صحيح httpversion = proxy_http_ver_major(ver);
  حرف *authority = عدم;
  حرف *رستہ = عدم;
  حرف *encoded_host = عدم;
  بنية httpreq *req = عدم;
  CURLcode نتیجہ;

  إذا(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->پورٹ);
  إذا(!authority) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  إذا(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ثابت حرف *s = dest->hostname;
    حرف *d;
    size_t hlen = لڑی_لمبائی(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    إذا(!encoded_host) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
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
    رستہ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (بنانشانی صحيح)dest->پورٹ);
  }
  وإلا {
    رستہ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (بنانشانی صحيح)dest->پورٹ);
  }

  إذا(!رستہ) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  إذا(ver == PROXY_HTTP_V1) {
    نتیجہ = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, لڑی_لمبائی(proxy_scheme),
                                authority, لڑی_لمبائی(authority),
                                رستہ, لڑی_لمبائی(رستہ));
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا إذا(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    نتیجہ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, لڑی_لمبائی(proxy_scheme),
                                authority, لڑی_لمبائی(authority),
                                رستہ, لڑی_لمبائی(رستہ));
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا {
    نتیجہ = CURLE_FAILED_INIT;
    اذهب باہرلا;
  }

  /* Setup the proxy-authorization header, if any */
  نتیجہ = Curl_http_output_auth(انکڑے, cf->conn, req->طریقہ, HTTPREQ_GET,
                                 req->authority, عدم, TRUE);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(انکڑے, cf->conn, STRCONST("Host"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Host", authority);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(انکڑے->req.hd_proxy_auth) {
    نتیجہ = Curl_dynhds_h1_cadd_سطر(&req->سرخیاں,
                                      انکڑے->req.hd_proxy_auth);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  ua = CURL_EASY_STR(انکڑے, STRING_USERAGENT);
  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(انکڑے, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "User-Agent", ua);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(انکڑے, cf->conn, STRCONST("Proxy-Connection"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Proxy-Connection", "Keep-Alive");
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(ver == PROXY_HTTP_V1) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Connection", "Upgrade");
    إذا(نتیجہ)
      اذهب باہرلا;

    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Upgrade", "connect-udp");
    إذا(نتیجہ)
      اذهب باہرلا;

    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Capsule-Protocol", "?1");
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, ":Protocol", "connect-udp");
    إذا(نتیجہ)
      اذهب باہرلا;

    إذا(ver >= PROXY_HTTP_V2) {
      نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Capsule-Protocol", "?1");
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }

  نتیجہ = dynhds_جوڑو_custom(انکڑے, TRUE, httpversion,
                             TRUE, &req->سرخیاں);

باہرلا:
  إذا(نتیجہ && req) {
    Curl_http_req_free(req);
    req = عدم;
  }
  curlx_free(authority);
  curlx_free(رستہ);
  curlx_free(encoded_host);
  *preq = req;
  أرجع نتیجہ;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    بنية httpreq **preq, بنية Curl_cfilter *cf,
    بنية Curl_easy *انکڑے, بنية Curl_peer *peer, بنية Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode نتیجہ = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, انکڑے, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, انکڑے, peer, dest, ver);
  إذا(نتیجہ)
    أرجع نتیجہ;

  infof(انکڑے, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->پورٹ);
  أرجع CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
    بنية http_resp *resp, bool udp_tunnel,
    proxy_inspect_نتیجہ *presult)
{
  بنية dynhds_اندراج *capsule_protocol = عدم;
  بنية dynhds_اندراج *auth_جواب = عدم;
  size_t i, سرخی_گنتی;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(resp);

  سرخی_گنتی = Curl_dynhds_گنتی(&resp->سرخیاں);
  infof(انکڑے, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->حالت);
  infof(انکڑے, "Response Headers (%zu total):", سرخی_گنتی);
  لكل(i = 0; i < سرخی_گنتی; i++) {
    بنية dynhds_اندراج *اندراج = Curl_dynhds_getn(&resp->سرخیاں, i);
    إذا(اندراج)
      infof(انکڑے, "  %s: %s", اندراج->name, اندراج->قدر);
  }

  إذا(resp->حالت == 407) {
    auth_جواب = Curl_dynhds_cget(&resp->سرخیاں, "Proxy-Authenticate");
  }

  إذا(auth_جواب) {
    CURL_TRC_CF(انکڑے, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_جواب->قدر);
    نتیجہ = Curl_http_داخلہ_auth(انکڑے, resp->حالت == 407,
                                  auth_جواب->قدر);
    إذا(نتیجہ)
      أرجع نتیجہ;
    إذا(انکڑے->req.newurl) {
      curlx_safefree(انکڑے->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      أرجع CURLE_OK;
    }
  }

  إذا(udp_tunnel) {
    إذا(resp->حالت / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->سرخیاں,
                                           "capsule-protocol");
      إذا(capsule_protocol) {
        إذا(!لڑی_مقابلہ(capsule_protocol->قدر, "?1", 2) &&
           !capsule_protocol->قدر[2]) {
          infof(انکڑے, "CONNECT-UDP tunnel established, response %d",
                resp->حالت);
          *presult = PROXY_INSPECT_OK;
          أرجع CURLE_OK;
        }
        failf(انکڑے, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->حالت, capsule_protocol->قدر);
        *presult = PROXY_INSPECT_FAILED;
        أرجع CURLE_COULDNT_CONNECT;
      }
      وإلا {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(انکڑے, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->حالت);
        *presult = PROXY_INSPECT_OK;
        أرجع CURLE_OK;
      }
    }
    وإلا {
      failf(انکڑے, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->حالت);
      *presult = PROXY_INSPECT_FAILED;
      أرجع CURLE_COULDNT_CONNECT;
    }
  }

  إذا(resp->حالت / 100 == 2) {
    infof(انکڑے, "CONNECT tunnel established, response %d", resp->حالت);
    *presult = PROXY_INSPECT_OK;
    أرجع CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  أرجع CURLE_COULDNT_CONNECT;
}

ساكن CURLcode http_proxy_cf_connect(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے,
                                      bool *پورا)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  CURL_TRC_CF(انکڑے, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  إذا(cf->اگلا) {
    نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, انکڑے, پورا);
    إذا(نتیجہ || !*پورا)
      أرجع نتیجہ;
  }

  *پورا = FALSE;
  إذا(!ctx->sub_filter_installed) {
    ثابت حرف *alpn = عدم;

    /* in case of h3_proxy, cf->next will be NULL initially */
    إذا(cf->اگلا) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->اگلا, انکڑے);
    }

    إذا(alpn)
      infof(انکڑے, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    وإلا إذا(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(انکڑے, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
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
      CURL_TRC_CF(انکڑے, cf, "installing subfilter for HTTP/1.0");
      نتیجہ = Curl_cf_h1_proxy_داخل_کرو_after(cf, انکڑے, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
    وإلا إذا(!strcmp(alpn, "http/1.1")) {
      صحيح httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(انکڑے, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      نتیجہ = Curl_cf_h1_proxy_داخل_کرو_after(cf, انکڑے, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
#جے_تعریف_شدہ USE_NGHTTP2
    وإلا إذا(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(انکڑے, cf, "installing subfilter for HTTP/2");
      نتیجہ = Curl_cf_h2_proxy_داخل_کرو_after(cf, انکڑے, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
#جے_انت /* USE_NGHTTP2 */
#إذا defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    وإلا إذا(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(انکڑے, cf, "installing subfilter for HTTP/3");
      نتیجہ = Curl_cf_h3_proxy_داخل_کرو_after(cf, انکڑے, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
#جے_انت /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    وإلا {
      failf(انکڑے, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      نتیجہ = CURLE_COULDNT_CONNECT;
      اذهب باہرلا;
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
    نتیجہ = CURLE_OK;
  }

باہرلا:
  إذا(!نتیجہ) {
    cf->connected = TRUE;
    *پورا = TRUE;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_http_proxy_query(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  بدل(query) {
  حالة CF_QUERY_HOST_پورٹ:
    *pres1 = (صحيح)ctx->tunnel_peer->پورٹ;
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
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
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
                                  بنية Curl_easy *انکڑے)
{
  بنية cf_proxy_ctx *ctx = cf->ctx;
  إذا(ctx) {
    CURL_TRC_CF(انکڑے, cf, "destroy");
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
  Curl_cf_def_انکڑے_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                         بنية Curl_easy *انکڑے,
                                         بنية Curl_peer *peer,
                                         بنية Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  بنية Curl_cfilter *cf;
  بنية cf_proxy_ctx *ctx = عدم;
  CURLcode نتیجہ;

  (فراغ)انکڑے;
  إذا(!peer || !tunnel_peer)
    أرجع CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  ctx = عدم;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

باہرلا:
  cf_https_proxy_ctx_free(ctx);
  أرجع نتیجہ;
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

#جے_انت /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
