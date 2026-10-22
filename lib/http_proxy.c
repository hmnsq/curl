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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-h1-proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-h2-proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

ਸਥਿਰ CURLcode dynhds_ਜੋੜੋ_custom(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  bool is_connect, ਪੂਰਨ httpversion,
                                  bool is_udp, ਬਣਤਰ dynhds *hds)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ curl_slist *h[2];
  ਬਣਤਰ curl_slist *ਸਿਰਲੇਖਾਂ;
  ਪੂਰਨ numlists = 1; /* by default */
  ਪੂਰਨ i;

  ਗਿਣਤੀ Curl_proxy_use proxy;

  ਜੇ(is_connect && !is_udp)
    proxy = ਸਿਰਲੇਖ_CONNECT;
  ਨਹੀਂਤਾਂ ਜੇ(is_connect && is_udp)
    proxy = ਸਿਰਲੇਖ_CONNECT_UDP;
  ਨਹੀਂਤਾਂ
    proxy = conn->ਬਿੱਟ.origin_is_proxy ? ਸਿਰਲੇਖ_PROXY : ਸਿਰਲੇਖ_SERVER;

  ਚੋਣ(proxy) {
  ਹਾਲਤ ਸਿਰਲੇਖ_SERVER:
    h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਰੋਕੋ;
  ਹਾਲਤ ਸਿਰਲੇਖ_PROXY:
    h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.sep_ਸਿਰਲੇਖਾਂ) {
      h[1] = ਅੰਕੜੇ->ਸੈੱਟ.proxyheaders;
      numlists++;
    }
    ਰੋਕੋ;
  ਹਾਲਤ ਸਿਰਲੇਖ_CONNECT:
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.sep_ਸਿਰਲੇਖਾਂ)
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.proxyheaders;
    ਨਹੀਂਤਾਂ
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਰੋਕੋ;
  ਹਾਲਤ ਸਿਰਲੇਖ_CONNECT_UDP:
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.sep_ਸਿਰਲੇਖਾਂ)
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.proxyheaders;
    ਨਹੀਂਤਾਂ
      h[0] = ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ;
    ਰੋਕੋ;
  }

  /* loop through one or two lists */
  ਲਈ(i = 0; i < numlists; i++) {
    ਲਈ(ਸਿਰਲੇਖਾਂ = h[i]; ਸਿਰਲੇਖਾਂ; ਸਿਰਲੇਖਾਂ = ਸਿਰਲੇਖਾਂ->ਅਗਲਾ) {
      ਬਣਤਰ Curl_str name;
      ਅਚਲ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
      size_t valuelen = 0;
      ਅਚਲ ਅੱਖਰ *ptr = ਸਿਰਲੇਖਾਂ->ਅੰਕੜੇ;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ਜੇ(!curlx_str_cspn(&ptr, &name, ";:")) {
        ਜੇ(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ਜੇ(*ptr) {
            ਮੁੱਲ = ptr;
            valuelen = ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ);
          }
          ਨਹੀਂਤਾਂ {
            /* quirk #1, suppress this header */
            ਜਾਰੀ;
          }
        }
        ਨਹੀਂਤਾਂ ਜੇ(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ਜੇ(!*ptr) {
            /* quirk #2, send an empty header */
            ਮੁੱਲ = "";
            valuelen = 0;
          }
          ਨਹੀਂਤਾਂ {
            /* this may be used for something else in the future,
             * ignore this for now */
            ਜਾਰੀ;
          }
        }
        ਨਹੀਂਤਾਂ
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ
        /* no name, move on */
        ਜਾਰੀ;

      DEBUGASSERT(curlx_strlen(&name) && ਮੁੱਲ);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ਜੇ(!curlx_strlen(&name))
        ਜਾਰੀ;
      ਜੇ(ਅੰਕੜੇ->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ਨਹੀਂਤਾਂ ਜੇ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      ਨਹੀਂਤਾਂ ਜੇ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ))
        ;
      ਨਹੀਂਤਾਂ {
        CURLcode ਨਤੀਜਾ =
          Curl_dynhds_ਜੋੜੋ(hds, curlx_str(&name), curlx_strlen(&name),
                          ਮੁੱਲ, valuelen);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }

  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ cf_proxy_ctx {
  ਬਣਤਰ Curl_peer *peer; /* proxy */
  ਬਣਤਰ Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  ਦੋਆਧਾਰੀ_ਅੰਕ(sub_filter_installed);
};

ਸਥਿਰ ਪੂਰਨ proxy_http_ver_major(proxy_http_ver ver)
{
  ਚੋਣ(ver) {
  ਹਾਲਤ PROXY_HTTP_V1:
    ਵਾਪਸ 11;
  ਹਾਲਤ PROXY_HTTP_V2:
    ਵਾਪਸ 20;
  ਹਾਲਤ PROXY_HTTP_V3:
    ਵਾਪਸ 30;
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLcode http_proxy_create_CONNECT(ਬਣਤਰ httpreq **preq,
                                          ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ Curl_peer *peer,
                                          ਬਣਤਰ Curl_peer *dest,
                                          proxy_http_ver ver)
{
  ਅੱਖਰ *authority = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *ua;
  ਪੂਰਨ httpversion = proxy_http_ver_major(ver);
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ httpreq *req = ਨਿਰਾ;

  (ਖਾਲੀ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->ਪੋਰਟ);
  ਜੇ(!authority) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ਨਿਰਾ, 0, authority, ਲੜੀ_ਲੰਬਾਈ(authority),
                              ਨਿਰਾ, 0);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* Setup the proxy-authorization header, if any */
  ਨਤੀਜਾ = Curl_http_output_auth(ਅੰਕੜੇ, cf->conn, req->ਵਿਧੀ, HTTPREQ_GET,
                                 req->authority, ਨਿਰਾ, TRUE);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ਜੇ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ਅੰਕੜੇ, cf->conn, STRCONST("Host"))) {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Host", authority);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ਅੰਕੜੇ->req.hd_proxy_auth) {
    ਨਤੀਜਾ = Curl_dynhds_h1_cadd_ਸਤਰ(&req->ਸਿਰਲੇਖਾਂ,
                                      ਅੰਕੜੇ->req.hd_proxy_auth);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ua = CURL_EASY_STR(ਅੰਕੜੇ, STRING_USERAGENT);
  ਜੇ(!Curl_checkProxyheaders(ਅੰਕੜੇ, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "User-Agent", ua);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ਅੰਕੜੇ, cf->conn, STRCONST("Proxy-Connection"))) {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Proxy-Connection", "Keep-Alive");
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = dynhds_ਜੋੜੋ_custom(ਅੰਕੜੇ, TRUE, httpversion,
                             FALSE, &req->ਸਿਰਲੇਖਾਂ);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && req) {
    Curl_http_req_free(req);
    req = ਨਿਰਾ;
  }
  curlx_free(authority);
  *preq = req;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode http_proxy_create_CONNECTUDP(ਬਣਤਰ httpreq **preq,
                                             ਬਣਤਰ Curl_cfilter *cf,
                                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_peer *peer,
                                             ਬਣਤਰ Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ਅਚਲ ਅੱਖਰ *proxy_scheme = "http", *ua;
  ਪੂਰਨ httpversion = proxy_http_ver_major(ver);
  ਅੱਖਰ *authority = ਨਿਰਾ;
  ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;
  ਅੱਖਰ *encoded_host = ਨਿਰਾ;
  ਬਣਤਰ httpreq *req = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->ਪੋਰਟ);
  ਜੇ(!authority) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ਅਚਲ ਅੱਖਰ *s = dest->hostname;
    ਅੱਖਰ *d;
    size_t hlen = ਲੜੀ_ਲੰਬਾਈ(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ਜੇ(!encoded_host) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    d = encoded_host;
    ਜਦੋਂ(*s) {
      ਜੇ(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      ਨਹੀਂਤਾਂ
        *d++ = *s;
      s++;
    }
    *d = '\0';
    ਰਸਤਾ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)dest->ਪੋਰਟ);
  }
  ਨਹੀਂਤਾਂ {
    ਰਸਤਾ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)dest->ਪੋਰਟ);
  }

  ਜੇ(!ਰਸਤਾ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ver == PROXY_HTTP_V1) {
    ਨਤੀਜਾ = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, ਲੜੀ_ਲੰਬਾਈ(proxy_scheme),
                                authority, ਲੜੀ_ਲੰਬਾਈ(authority),
                                ਰਸਤਾ, ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ));
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    ਨਤੀਜਾ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, ਲੜੀ_ਲੰਬਾਈ(proxy_scheme),
                                authority, ਲੜੀ_ਲੰਬਾਈ(authority),
                                ਰਸਤਾ, ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ));
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Setup the proxy-authorization header, if any */
  ਨਤੀਜਾ = Curl_http_output_auth(ਅੰਕੜੇ, cf->conn, req->ਵਿਧੀ, HTTPREQ_GET,
                                 req->authority, ਨਿਰਾ, TRUE);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ਜੇ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ਅੰਕੜੇ, cf->conn, STRCONST("Host"))) {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Host", authority);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ਅੰਕੜੇ->req.hd_proxy_auth) {
    ਨਤੀਜਾ = Curl_dynhds_h1_cadd_ਸਤਰ(&req->ਸਿਰਲੇਖਾਂ,
                                      ਅੰਕੜੇ->req.hd_proxy_auth);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ua = CURL_EASY_STR(ਅੰਕੜੇ, STRING_USERAGENT);
  ਜੇ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ਅੰਕੜੇ, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "User-Agent", ua);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ਅੰਕੜੇ, cf->conn, STRCONST("Proxy-Connection"))) {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Proxy-Connection", "Keep-Alive");
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ver == PROXY_HTTP_V1) {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Connection", "Upgrade");
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Upgrade", "connect-udp");
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Capsule-Protocol", "?1");
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, ":Protocol", "connect-udp");
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਜੇ(ver >= PROXY_HTTP_V2) {
      ਨਤੀਜਾ = Curl_dynhds_cadd(&req->ਸਿਰਲੇਖਾਂ, "Capsule-Protocol", "?1");
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਨਤੀਜਾ = dynhds_ਜੋੜੋ_custom(ਅੰਕੜੇ, TRUE, httpversion,
                             TRUE, &req->ਸਿਰਲੇਖਾਂ);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && req) {
    Curl_http_req_free(req);
    req = ਨਿਰਾ;
  }
  curlx_free(authority);
  curlx_free(ਰਸਤਾ);
  curlx_free(encoded_host);
  *preq = req;
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    ਬਣਤਰ httpreq **preq, ਬਣਤਰ Curl_cfilter *cf,
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_peer *peer, ਬਣਤਰ Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode ਨਤੀਜਾ = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, ਅੰਕੜੇ, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, ਅੰਕੜੇ, peer, dest, ver);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  infof(ਅੰਕੜੇ, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->ਪੋਰਟ);
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
    ਬਣਤਰ http_resp *resp, bool udp_tunnel,
    proxy_inspect_ਨਤੀਜਾ *presult)
{
  ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *capsule_protocol = ਨਿਰਾ;
  ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *auth_ਜਵਾਬ = ਨਿਰਾ;
  size_t i, ਸਿਰਲੇਖ_ਗਿਣਤੀ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(resp);

  ਸਿਰਲੇਖ_ਗਿਣਤੀ = Curl_dynhds_ਗਿਣਤੀ(&resp->ਸਿਰਲੇਖਾਂ);
  infof(ਅੰਕੜੇ, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->ਸਥਿਤੀ);
  infof(ਅੰਕੜੇ, "Response Headers (%zu total):", ਸਿਰਲੇਖ_ਗਿਣਤੀ);
  ਲਈ(i = 0; i < ਸਿਰਲੇਖ_ਗਿਣਤੀ; i++) {
    ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ = Curl_dynhds_getn(&resp->ਸਿਰਲੇਖਾਂ, i);
    ਜੇ(ਇੰਦਰਾਜ)
      infof(ਅੰਕੜੇ, "  %s: %s", ਇੰਦਰਾਜ->name, ਇੰਦਰਾਜ->ਮੁੱਲ);
  }

  ਜੇ(resp->ਸਥਿਤੀ == 407) {
    auth_ਜਵਾਬ = Curl_dynhds_cget(&resp->ਸਿਰਲੇਖਾਂ, "Proxy-Authenticate");
  }

  ਜੇ(auth_ਜਵਾਬ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_ਜਵਾਬ->ਮੁੱਲ);
    ਨਤੀਜਾ = Curl_http_ਨਿਵੇਸ਼_auth(ਅੰਕੜੇ, resp->ਸਥਿਤੀ == 407,
                                  auth_ਜਵਾਬ->ਮੁੱਲ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(ਅੰਕੜੇ->req.newurl) {
      curlx_safefree(ਅੰਕੜੇ->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      ਵਾਪਸ CURLE_OK;
    }
  }

  ਜੇ(udp_tunnel) {
    ਜੇ(resp->ਸਥਿਤੀ / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->ਸਿਰਲੇਖਾਂ,
                                           "capsule-protocol");
      ਜੇ(capsule_protocol) {
        ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(capsule_protocol->ਮੁੱਲ, "?1", 2) &&
           !capsule_protocol->ਮੁੱਲ[2]) {
          infof(ਅੰਕੜੇ, "CONNECT-UDP tunnel established, response %d",
                resp->ਸਥਿਤੀ);
          *presult = PROXY_INSPECT_OK;
          ਵਾਪਸ CURLE_OK;
        }
        failf(ਅੰਕੜੇ, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->ਸਥਿਤੀ, capsule_protocol->ਮੁੱਲ);
        *presult = PROXY_INSPECT_FAILED;
        ਵਾਪਸ CURLE_COULDNT_CONNECT;
      }
      ਨਹੀਂਤਾਂ {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(ਅੰਕੜੇ, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->ਸਥਿਤੀ);
        *presult = PROXY_INSPECT_OK;
        ਵਾਪਸ CURLE_OK;
      }
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->ਸਥਿਤੀ);
      *presult = PROXY_INSPECT_FAILED;
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
  }

  ਜੇ(resp->ਸਥਿਤੀ / 100 == 2) {
    infof(ਅੰਕੜੇ, "CONNECT tunnel established, response %d", resp->ਸਥਿਤੀ);
    *presult = PROXY_INSPECT_OK;
    ਵਾਪਸ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  ਵਾਪਸ CURLE_COULDNT_CONNECT;
}

ਸਥਿਰ CURLcode http_proxy_cf_connect(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ਜੇ(cf->ਅਗਲਾ) {
    ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  *ਪੂਰਾ = FALSE;
  ਜੇ(!ctx->sub_filter_installed) {
    ਅਚਲ ਅੱਖਰ *alpn = ਨਿਰਾ;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ਜੇ(cf->ਅਗਲਾ) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->ਅਗਲਾ, ਅੰਕੜੇ);
    }

    ਜੇ(alpn)
      infof(ਅੰਕੜੇ, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    ਨਹੀਂਤਾਂ ਜੇ(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(ਅੰਕੜੇ, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      ਚੋਣ(ctx->proxytype) {
      ਹਾਲਤ CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        ਰੋਕੋ;
      ਹਾਲਤ CURLPROXY_HTTPS2:
        alpn = "h2";
        ਰੋਕੋ;
      ਹਾਲਤ CURLPROXY_HTTPS3:
        alpn = "h3";
        ਰੋਕੋ;
      ਮੂਲ:
        alpn = "http/1.1";
        ਰੋਕੋ;
      }
    }

    ਜੇ(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "installing subfilter for HTTP/1.0");
      ਨਤੀਜਾ = Curl_cf_h1_proxy_ਪਾਓ_after(cf, ਅੰਕੜੇ, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!strcmp(alpn, "http/1.1")) {
      ਪੂਰਨ httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      ਨਤੀਜਾ = Curl_cf_h1_proxy_ਪਾਓ_after(cf, ਅੰਕੜੇ, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
    ਨਹੀਂਤਾਂ ਜੇ(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "installing subfilter for HTTP/2");
      ਨਤੀਜਾ = Curl_cf_h2_proxy_ਪਾਓ_after(cf, ਅੰਕੜੇ, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
#ਜੇ_ਅੰਤ /* USE_NGHTTP2 */
#ਜੇ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    ਨਹੀਂਤਾਂ ਜੇ(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "installing subfilter for HTTP/3");
      ਨਤੀਜਾ = Curl_cf_h3_proxy_ਪਾਓ_after(cf, ਅੰਕੜੇ, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
#ਜੇ_ਅੰਤ /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    ਜਾਓ connect_sub;
  }
  ਨਹੀਂਤਾਂ {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    ਨਤੀਜਾ = CURLE_OK;
  }

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ) {
    cf->connected = TRUE;
    *ਪੂਰਾ = TRUE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_http_proxy_query(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_proxy_ctx *ctx = cf->ctx;
  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_HOST_ਪੋਰਟ:
    *pres1 = (ਪੂਰਨ)ctx->tunnel_peer->ਪੋਰਟ;
    *((ਅਚਲ ਅੱਖਰ **)pres2) = ctx->tunnel_peer->hostname;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_ALPN_NEGOTIATED: {
    ਅਚਲ ਅੱਖਰ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ਨਿਰਾ;
    ਵਾਪਸ CURLE_OK;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ ਖਾਲੀ cf_https_proxy_ctx_free(ਬਣਤਰ cf_proxy_ctx *ctx)
{
  ਜੇ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ਸਥਿਰ ਖਾਲੀ http_proxy_cf_destroy(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_proxy_ctx *ctx = cf->ctx;
  ਜੇ(ctx) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

ਬਣਤਰ Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_ਤਿਆਰੀ,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_peer *peer,
                                         ਬਣਤਰ Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ cf_proxy_ctx *ctx = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(!peer || !tunnel_peer)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = ਨਿਰਾ;
  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);

ਬਾਹਰਲਾ:
  cf_https_proxy_ctx_free(ctx);
  ਵਾਪਸ ਨਤੀਜਾ;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  ਚੋਣ(proxytype) {
  ਹਾਲਤ CURLPROXY_HTTPS3:
    ਵਾਪਸ TRNSPRT_QUIC;
  ਮੂਲ:
    ਵਾਪਸ TRNSPRT_TCP;
  }
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
