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
#شامل "curl_setup.h"

#شامل "http_proxy.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#شامل "curl_trc.h"
#شامل "http.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "cf-h1-proxy.h"
#شامل "cf-h2-proxy.h"
#شامل "connect.h"
#شامل "vauth/vauth.h"
#شامل "vquic/vquic.h"
#شامل "curlx/strparse.h"

ساکن CURLcode dynhds_جوڑو_custom(ساخت Curl_easy *ڈیٹا,
                                  bool is_connect, صحیح httpversion,
                                  bool is_udp, ساخت dynhds *hds)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت curl_slist *h[2];
  ساخت curl_slist *سرخیاں;
  صحیح numlists = 1; /* by default */
  صحیح i;

  شمار Curl_proxy_use proxy;

  اگر(is_connect && !is_udp)
    proxy = سرخی_CONNECT;
  ورنہ اگر(is_connect && is_udp)
    proxy = سرخی_CONNECT_UDP;
  ورنہ
    proxy = conn->بٹ.origin_is_proxy ? سرخی_PROXY : سرخی_SERVER;

  انتخاب(proxy) {
  حالت سرخی_SERVER:
    h[0] = ڈیٹا->سیٹ.سرخیاں;
    توڑو;
  حالت سرخی_PROXY:
    h[0] = ڈیٹا->سیٹ.سرخیاں;
    اگر(ڈیٹا->سیٹ.sep_سرخیاں) {
      h[1] = ڈیٹا->سیٹ.proxyheaders;
      numlists++;
    }
    توڑو;
  حالت سرخی_CONNECT:
    اگر(ڈیٹا->سیٹ.sep_سرخیاں)
      h[0] = ڈیٹا->سیٹ.proxyheaders;
    ورنہ
      h[0] = ڈیٹا->سیٹ.سرخیاں;
    توڑو;
  حالت سرخی_CONNECT_UDP:
    اگر(ڈیٹا->سیٹ.sep_سرخیاں)
      h[0] = ڈیٹا->سیٹ.proxyheaders;
    ورنہ
      h[0] = ڈیٹا->سیٹ.سرخیاں;
    توڑو;
  }

  /* loop through one or two lists */
  برائے(i = 0; i < numlists; i++) {
    برائے(سرخیاں = h[i]; سرخیاں; سرخیاں = سرخیاں->اگلا) {
      ساخت Curl_str name;
      ثابت حرف *قدر = تہی;
      size_t valuelen = 0;
      ثابت حرف *ptr = سرخیاں->ڈیٹا;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      اگر(!curlx_str_cspn(&ptr, &name, ";:")) {
        اگر(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          اگر(*ptr) {
            قدر = ptr;
            valuelen = لڑی_لمبائی(قدر);
          }
          ورنہ {
            /* quirk #1, suppress this header */
            جاری;
          }
        }
        ورنہ اگر(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          اگر(!*ptr) {
            /* quirk #2, send an empty header */
            قدر = "";
            valuelen = 0;
          }
          ورنہ {
            /* this may be used for something else in the future,
             * ignore this for now */
            جاری;
          }
        }
        ورنہ
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          جاری;
      }
      ورنہ
        /* no name, move on */
        جاری;

      DEBUGASSERT(curlx_strlen(&name) && قدر);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      اگر(!curlx_strlen(&name))
        جاری;
      اگر(ڈیٹا->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ورنہ اگر(ڈیٹا->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ورنہ اگر(ڈیٹا->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ورنہ اگر(ڈیٹا->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ورنہ اگر((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      ورنہ اگر((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_مقصد_host(ڈیٹا))
        ;
      ورنہ {
        CURLcode نتیجہ =
          Curl_dynhds_جوڑو(hds, curlx_str(&name), curlx_strlen(&name),
                          قدر, valuelen);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
    }
  }

  واپس CURLE_OK;
}

ساخت cf_proxy_ctx {
  ساخت Curl_peer *peer; /* proxy */
  ساخت Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  ثنائی_ہندسہ(sub_filter_installed);
};

ساکن صحیح proxy_http_ver_major(proxy_http_ver ver)
{
  انتخاب(ver) {
  حالت PROXY_HTTP_V1:
    واپس 11;
  حالت PROXY_HTTP_V2:
    واپس 20;
  حالت PROXY_HTTP_V3:
    واپس 30;
  }
  واپس 0;
}

ساکن CURLcode http_proxy_create_CONNECT(ساخت httpreq **preq,
                                          ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          ساخت Curl_peer *peer,
                                          ساخت Curl_peer *dest,
                                          proxy_http_ver ver)
{
  حرف *authority = تہی;
  ثابت حرف *ua;
  صحیح httpversion = proxy_http_ver_major(ver);
  CURLcode نتیجہ;
  ساخت httpreq *req = تہی;

  (خالی)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->پورٹ);
  اگر(!authority) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  نتیجہ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              تہی, 0, authority, لڑی_لمبائی(authority),
                              تہی, 0);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* Setup the proxy-authorization header, if any */
  نتیجہ = Curl_http_output_auth(ڈیٹا, cf->conn, req->طریقہ, HTTPREQ_GET,
                                 req->authority, تہی, TRUE);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ڈیٹا, cf->conn, STRCONST("Host"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Host", authority);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(ڈیٹا->req.hd_proxy_auth) {
    نتیجہ = Curl_dynhds_h1_cadd_سطر(&req->سرخیاں,
                                      ڈیٹا->req.hd_proxy_auth);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  ua = CURL_EASY_STR(ڈیٹا, STRING_USERAGENT);
  اگر(!Curl_checkProxyheaders(ڈیٹا, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "User-Agent", ua);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ڈیٹا, cf->conn, STRCONST("Proxy-Connection"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Proxy-Connection", "Keep-Alive");
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  نتیجہ = dynhds_جوڑو_custom(ڈیٹا, TRUE, httpversion,
                             FALSE, &req->سرخیاں);

برآمد:
  اگر(نتیجہ && req) {
    Curl_http_req_free(req);
    req = تہی;
  }
  curlx_free(authority);
  *preq = req;
  واپس نتیجہ;
}

ساکن CURLcode http_proxy_create_CONNECTUDP(ساخت httpreq **preq,
                                             ساخت Curl_cfilter *cf,
                                             ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_peer *peer,
                                             ساخت Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ثابت حرف *proxy_scheme = "http", *ua;
  صحیح httpversion = proxy_http_ver_major(ver);
  حرف *authority = تہی;
  حرف *راستہ = تہی;
  حرف *encoded_host = تہی;
  ساخت httpreq *req = تہی;
  CURLcode نتیجہ;

  اگر(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->پورٹ);
  اگر(!authority) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  اگر(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ثابت حرف *s = dest->hostname;
    حرف *d;
    size_t hlen = لڑی_لمبائی(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    اگر(!encoded_host) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
    d = encoded_host;
    جب(*s) {
      اگر(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      ورنہ
        *d++ = *s;
      s++;
    }
    *d = '\0';
    راستہ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (غیرعلامتی صحیح)dest->پورٹ);
  }
  ورنہ {
    راستہ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (غیرعلامتی صحیح)dest->پورٹ);
  }

  اگر(!راستہ) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  اگر(ver == PROXY_HTTP_V1) {
    نتیجہ = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, لڑی_لمبائی(proxy_scheme),
                                authority, لڑی_لمبائی(authority),
                                راستہ, لڑی_لمبائی(راستہ));
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ اگر(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    نتیجہ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, لڑی_لمبائی(proxy_scheme),
                                authority, لڑی_لمبائی(authority),
                                راستہ, لڑی_لمبائی(راستہ));
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ {
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  /* Setup the proxy-authorization header, if any */
  نتیجہ = Curl_http_output_auth(ڈیٹا, cf->conn, req->طریقہ, HTTPREQ_GET,
                                 req->authority, تہی, TRUE);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ڈیٹا, cf->conn, STRCONST("Host"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Host", authority);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(ڈیٹا->req.hd_proxy_auth) {
    نتیجہ = Curl_dynhds_h1_cadd_سطر(&req->سرخیاں,
                                      ڈیٹا->req.hd_proxy_auth);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  ua = CURL_EASY_STR(ڈیٹا, STRING_USERAGENT);
  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ڈیٹا, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "User-Agent", ua);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ڈیٹا, cf->conn, STRCONST("Proxy-Connection"))) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Proxy-Connection", "Keep-Alive");
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(ver == PROXY_HTTP_V1) {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Connection", "Upgrade");
    اگر(نتیجہ)
      جاؤ برآمد;

    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Upgrade", "connect-udp");
    اگر(نتیجہ)
      جاؤ برآمد;

    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Capsule-Protocol", "?1");
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ {
    نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, ":Protocol", "connect-udp");
    اگر(نتیجہ)
      جاؤ برآمد;

    اگر(ver >= PROXY_HTTP_V2) {
      نتیجہ = Curl_dynhds_cadd(&req->سرخیاں, "Capsule-Protocol", "?1");
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }

  نتیجہ = dynhds_جوڑو_custom(ڈیٹا, TRUE, httpversion,
                             TRUE, &req->سرخیاں);

برآمد:
  اگر(نتیجہ && req) {
    Curl_http_req_free(req);
    req = تہی;
  }
  curlx_free(authority);
  curlx_free(راستہ);
  curlx_free(encoded_host);
  *preq = req;
  واپس نتیجہ;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    ساخت httpreq **preq, ساخت Curl_cfilter *cf,
    ساخت Curl_easy *ڈیٹا, ساخت Curl_peer *peer, ساخت Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode نتیجہ = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, ڈیٹا, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, ڈیٹا, peer, dest, ver);
  اگر(نتیجہ)
    واپس نتیجہ;

  infof(ڈیٹا, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->پورٹ);
  واپس CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
    ساخت http_resp *resp, bool udp_tunnel,
    proxy_inspect_نتیجہ *presult)
{
  ساخت dynhds_اندراج *capsule_protocol = تہی;
  ساخت dynhds_اندراج *auth_جواب = تہی;
  size_t i, سرخی_گنتی;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(resp);

  سرخی_گنتی = Curl_dynhds_گنتی(&resp->سرخیاں);
  infof(ڈیٹا, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->حالت_2);
  infof(ڈیٹا, "Response Headers (%zu total):", سرخی_گنتی);
  برائے(i = 0; i < سرخی_گنتی; i++) {
    ساخت dynhds_اندراج *اندراج_2 = Curl_dynhds_getn(&resp->سرخیاں, i);
    اگر(اندراج_2)
      infof(ڈیٹا, "  %s: %s", اندراج_2->name, اندراج_2->قدر);
  }

  اگر(resp->حالت_2 == 407) {
    auth_جواب = Curl_dynhds_cget(&resp->سرخیاں, "Proxy-Authenticate");
  }

  اگر(auth_جواب) {
    CURL_TRC_CF(ڈیٹا, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_جواب->قدر);
    نتیجہ = Curl_http_داخلہ_auth(ڈیٹا, resp->حالت_2 == 407,
                                  auth_جواب->قدر);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(ڈیٹا->req.newurl) {
      curlx_safefree(ڈیٹا->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      واپس CURLE_OK;
    }
  }

  اگر(udp_tunnel) {
    اگر(resp->حالت_2 / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->سرخیاں,
                                           "capsule-protocol");
      اگر(capsule_protocol) {
        اگر(!لڑی_موازنہ(capsule_protocol->قدر, "?1", 2) &&
           !capsule_protocol->قدر[2]) {
          infof(ڈیٹا, "CONNECT-UDP tunnel established, response %d",
                resp->حالت_2);
          *presult = PROXY_INSPECT_OK;
          واپس CURLE_OK;
        }
        failf(ڈیٹا, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->حالت_2, capsule_protocol->قدر);
        *presult = PROXY_INSPECT_FAILED;
        واپس CURLE_COULDNT_CONNECT;
      }
      ورنہ {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(ڈیٹا, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->حالت_2);
        *presult = PROXY_INSPECT_OK;
        واپس CURLE_OK;
      }
    }
    ورنہ {
      failf(ڈیٹا, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->حالت_2);
      *presult = PROXY_INSPECT_FAILED;
      واپس CURLE_COULDNT_CONNECT;
    }
  }

  اگر(resp->حالت_2 / 100 == 2) {
    infof(ڈیٹا, "CONNECT tunnel established, response %d", resp->حالت_2);
    *presult = PROXY_INSPECT_OK;
    واپس CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  واپس CURLE_COULDNT_CONNECT;
}

ساکن CURLcode http_proxy_cf_connect(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      bool *مکمل)
{
  ساخت cf_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  CURL_TRC_CF(ڈیٹا, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  اگر(cf->اگلا) {
    نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  *مکمل = FALSE;
  اگر(!ctx->sub_filter_installed) {
    ثابت حرف *alpn = تہی;

    /* in case of h3_proxy, cf->next will be NULL initially */
    اگر(cf->اگلا) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->اگلا, ڈیٹا);
    }

    اگر(alpn)
      infof(ڈیٹا, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    ورنہ اگر(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(ڈیٹا, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      انتخاب(ctx->proxytype) {
      حالت CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        توڑو;
      حالت CURLPROXY_HTTPS2:
        alpn = "h2";
        توڑو;
      حالت CURLPROXY_HTTPS3:
        alpn = "h3";
        توڑو;
      طےشدہ:
        alpn = "http/1.1";
        توڑو;
      }
    }

    اگر(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(ڈیٹا, cf, "installing subfilter for HTTP/1.0");
      نتیجہ = Curl_cf_h1_proxy_داخل_کرو_after(cf, ڈیٹا, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
    ورنہ اگر(!strcmp(alpn, "http/1.1")) {
      صحیح httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(ڈیٹا, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      نتیجہ = Curl_cf_h1_proxy_داخل_کرو_after(cf, ڈیٹا, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
#اگر_تعریف_شدہ USE_NGHTTP2
    ورنہ اگر(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(ڈیٹا, cf, "installing subfilter for HTTP/2");
      نتیجہ = Curl_cf_h2_proxy_داخل_کرو_after(cf, ڈیٹا, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
#اگر_ختم /* USE_NGHTTP2 */
#اگر defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    ورنہ اگر(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(ڈیٹا, cf, "installing subfilter for HTTP/3");
      نتیجہ = Curl_cf_h3_proxy_داخل_کرو_after(cf, ڈیٹا, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
#اگر_ختم /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    ورنہ {
      failf(ڈیٹا, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      نتیجہ = CURLE_COULDNT_CONNECT;
      جاؤ برآمد;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    جاؤ connect_sub;
  }
  ورنہ {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    نتیجہ = CURLE_OK;
  }

برآمد:
  اگر(!نتیجہ) {
    cf->connected = TRUE;
    *مکمل = TRUE;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_http_proxy_query(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_proxy_ctx *ctx = cf->ctx;
  انتخاب(query) {
  حالت CF_QUERY_HOST_پورٹ:
    *pres1 = (صحیح)ctx->tunnel_peer->پورٹ;
    *((ثابت حرف **)pres2) = ctx->tunnel_peer->hostname;
    واپس CURLE_OK;
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = تہی;
    واپس CURLE_OK;
  }
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساکن خالی cf_https_proxy_ctx_free(ساخت cf_proxy_ctx *ctx)
{
  اگر(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ساکن خالی http_proxy_cf_destroy(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_proxy_ctx *ctx = cf->ctx;
  اگر(ctx) {
    CURL_TRC_CF(ڈیٹا, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

ساخت Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_تیاری,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت Curl_peer *peer,
                                         ساخت Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  ساخت Curl_cfilter *cf;
  ساخت cf_proxy_ctx *ctx = تہی;
  CURLcode نتیجہ;

  (خالی)ڈیٹا;
  اگر(!peer || !tunnel_peer)
    واپس CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  ctx = تہی;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

برآمد:
  cf_https_proxy_ctx_free(ctx);
  واپس نتیجہ;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  انتخاب(proxytype) {
  حالت CURLPROXY_HTTPS3:
    واپس TRNSPRT_QUIC;
  طےشدہ:
    واپس TRNSPRT_TCP;
  }
}

#اگر_ختم /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
