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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "http_proxy.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-h1-proxy.h"
#ಅನ್ತರ್ಭಾವ "cf-h2-proxy.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಸ್ಥಿರ CURLcode dynhds_ಕೂಡಿಸು_custom(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool is_connect, ಪೂರ್ಣಾಂಕ httpversion,
                                  bool is_udp, ರಚನೆ dynhds *hds)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ curl_slist *h[2];
  ರಚನೆ curl_slist *ಶೀರ್ಷಿಕೆಗಳು;
  ಪೂರ್ಣಾಂಕ numlists = 1; /* by default */
  ಪೂರ್ಣಾಂಕ i;

  ಎಣಿಕೆ Curl_proxy_use proxy;

  ಆದರೆ(is_connect && !is_udp)
    proxy = ಶೀರ್ಷಿಕೆ_CONNECT;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(is_connect && is_udp)
    proxy = ಶೀರ್ಷಿಕೆ_CONNECT_UDP;
  ಇಲ್ಲದಿದ್ದರೆ
    proxy = conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy ? ಶೀರ್ಷಿಕೆ_PROXY : ಶೀರ್ಷಿಕೆ_SERVER;

  ಆಯ್ಕೆ(proxy) {
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_SERVER:
    h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_PROXY:
    h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.sep_ಶೀರ್ಷಿಕೆಗಳು) {
      h[1] = ದತ್ತಾಂಶ->ಗಣ.proxyheaders;
      numlists++;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_CONNECT:
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.sep_ಶೀರ್ಷಿಕೆಗಳು)
      h[0] = ದತ್ತಾಂಶ->ಗಣ.proxyheaders;
    ಇಲ್ಲದಿದ್ದರೆ
      h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ಶೀರ್ಷಿಕೆ_CONNECT_UDP:
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.sep_ಶೀರ್ಷಿಕೆಗಳು)
      h[0] = ದತ್ತಾಂಶ->ಗಣ.proxyheaders;
    ಇಲ್ಲದಿದ್ದರೆ
      h[0] = ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು;
    ನಿಲ್ಲಿಸು;
  }

  /* loop through one or two lists */
  ಗಾಗಿ(i = 0; i < numlists; i++) {
    ಗಾಗಿ(ಶೀರ್ಷಿಕೆಗಳು = h[i]; ಶೀರ್ಷಿಕೆಗಳು; ಶೀರ್ಷಿಕೆಗಳು = ಶೀರ್ಷಿಕೆಗಳು->ಮುಂದಿನ) {
      ರಚನೆ Curl_str name;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
      size_t valuelen = 0;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = ಶೀರ್ಷಿಕೆಗಳು->ದತ್ತಾಂಶ;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ಆದರೆ(!curlx_str_cspn(&ptr, &name, ";:")) {
        ಆದರೆ(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ಆದರೆ(*ptr) {
            ಮೌಲ್ಯ = ptr;
            valuelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ);
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            /* quirk #1, suppress this header */
            ಮುಂದುವರಿ;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ಆದರೆ(!*ptr) {
            /* quirk #2, send an empty header */
            ಮೌಲ್ಯ = "";
            valuelen = 0;
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            /* this may be used for something else in the future,
             * ignore this for now */
            ಮುಂದುವರಿ;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* no name, move on */
        ಮುಂದುವರಿ;

      DEBUGASSERT(curlx_strlen(&name) && ಮೌಲ್ಯ);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ಆದರೆ(!curlx_strlen(&name))
        ಮುಂದುವರಿ;
      ಆದರೆ(ದತ್ತಾಂಶ->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ))
        ;
      ಇಲ್ಲದಿದ್ದರೆ {
        CURLcode ಫಲಿತಾಂಶ =
          Curl_dynhds_ಕೂಡಿಸು(hds, curlx_str(&name), curlx_strlen(&name),
                          ಮೌಲ್ಯ, valuelen);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ cf_proxy_ctx {
  ರಚನೆ Curl_peer *peer; /* proxy */
  ರಚನೆ Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  ಬಿಟ್(sub_filter_installed);
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proxy_http_ver_major(proxy_http_ver ver)
{
  ಆಯ್ಕೆ(ver) {
  ಸ್ಥಿತಿ PROXY_HTTP_V1:
    ಹಿಂತಿರುಗಿ 11;
  ಸ್ಥಿತಿ PROXY_HTTP_V2:
    ಹಿಂತಿರುಗಿ 20;
  ಸ್ಥಿತಿ PROXY_HTTP_V3:
    ಹಿಂತಿರುಗಿ 30;
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLcode http_proxy_create_CONNECT(ರಚನೆ httpreq **preq,
                                          ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ Curl_peer *peer,
                                          ರಚನೆ Curl_peer *dest,
                                          proxy_http_ver ver)
{
  ಅಕ್ಷರ *authority = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ua;
  ಪೂರ್ಣಾಂಕ httpversion = proxy_http_ver_major(ver);
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ httpreq *req = ಖಾಲಿ;

  (ಶೂನ್ಯ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->ಪೋರ್ಟ್);
  ಆದರೆ(!authority) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ಖಾಲಿ, 0, authority, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(authority),
                              ಖಾಲಿ, 0);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* Setup the proxy-authorization header, if any */
  ಫಲಿತಾಂಶ = Curl_http_output_auth(ದತ್ತಾಂಶ, cf->conn, req->ವಿಧಾನ, HTTPREQ_GET,
                                 req->authority, ಖಾಲಿ, TRUE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ಆದರೆ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ದತ್ತಾಂಶ, cf->conn, STRCONST("Host"))) {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Host", authority);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->req.hd_proxy_auth) {
    ಫಲಿತಾಂಶ = Curl_dynhds_h1_cadd_ಸಾಲು(&req->ಶೀರ್ಷಿಕೆಗಳು,
                                      ದತ್ತಾಂಶ->req.hd_proxy_auth);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ua = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_USERAGENT);
  ಆದರೆ(!Curl_checkProxyheaders(ದತ್ತಾಂಶ, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "User-Agent", ua);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ದತ್ತಾಂಶ, cf->conn, STRCONST("Proxy-Connection"))) {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Proxy-Connection", "Keep-Alive");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = dynhds_ಕೂಡಿಸು_custom(ದತ್ತಾಂಶ, TRUE, httpversion,
                             FALSE, &req->ಶೀರ್ಷಿಕೆಗಳು);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && req) {
    Curl_http_req_free(req);
    req = ಖಾಲಿ;
  }
  curlx_free(authority);
  *preq = req;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode http_proxy_create_CONNECTUDP(ರಚನೆ httpreq **preq,
                                             ರಚನೆ Curl_cfilter *cf,
                                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_peer *peer,
                                             ರಚನೆ Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proxy_scheme = "http", *ua;
  ಪೂರ್ಣಾಂಕ httpversion = proxy_http_ver_major(ver);
  ಅಕ್ಷರ *authority = ಖಾಲಿ;
  ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;
  ಅಕ್ಷರ *encoded_host = ಖಾಲಿ;
  ರಚನೆ httpreq *req = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->ಪೋರ್ಟ್);
  ಆದರೆ(!authority) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = dest->hostname;
    ಅಕ್ಷರ *d;
    size_t hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ಆದರೆ(!encoded_host) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    d = encoded_host;
    ಆಗ(*s) {
      ಆದರೆ(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      ಇಲ್ಲದಿದ್ದರೆ
        *d++ = *s;
      s++;
    }
    *d = '\0';
    ಮಾರ್ಗ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)dest->ಪೋರ್ಟ್);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಮಾರ್ಗ = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)dest->ಪೋರ್ಟ್);
  }

  ಆದರೆ(!ಮಾರ್ಗ) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ver == PROXY_HTTP_V1) {
    ಫಲಿತಾಂಶ = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(proxy_scheme),
                                authority, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(authority),
                                ಮಾರ್ಗ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ));
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    ಫಲಿತಾಂಶ = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(proxy_scheme),
                                authority, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(authority),
                                ಮಾರ್ಗ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ));
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Setup the proxy-authorization header, if any */
  ಫಲಿತಾಂಶ = Curl_http_output_auth(ದತ್ತಾಂಶ, cf->conn, req->ವಿಧಾನ, HTTPREQ_GET,
                                 req->authority, ಖಾಲಿ, TRUE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ಆದರೆ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ದತ್ತಾಂಶ, cf->conn, STRCONST("Host"))) {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Host", authority);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->req.hd_proxy_auth) {
    ಫಲಿತಾಂಶ = Curl_dynhds_h1_cadd_ಸಾಲು(&req->ಶೀರ್ಷಿಕೆಗಳು,
                                      ದತ್ತಾಂಶ->req.hd_proxy_auth);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ua = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_USERAGENT);
  ಆದರೆ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ದತ್ತಾಂಶ, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "User-Agent", ua);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(ದತ್ತಾಂಶ, cf->conn, STRCONST("Proxy-Connection"))) {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Proxy-Connection", "Keep-Alive");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ver == PROXY_HTTP_V1) {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Connection", "Upgrade");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Upgrade", "connect-udp");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Capsule-Protocol", "?1");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, ":Protocol", "connect-udp");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಆದರೆ(ver >= PROXY_HTTP_V2) {
      ಫಲಿತಾಂಶ = Curl_dynhds_cadd(&req->ಶೀರ್ಷಿಕೆಗಳು, "Capsule-Protocol", "?1");
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಫಲಿತಾಂಶ = dynhds_ಕೂಡಿಸು_custom(ದತ್ತಾಂಶ, TRUE, httpversion,
                             TRUE, &req->ಶೀರ್ಷಿಕೆಗಳು);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && req) {
    Curl_http_req_free(req);
    req = ಖಾಲಿ;
  }
  curlx_free(authority);
  curlx_free(ಮಾರ್ಗ);
  curlx_free(encoded_host);
  *preq = req;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    ರಚನೆ httpreq **preq, ರಚನೆ Curl_cfilter *cf,
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_peer *peer, ರಚನೆ Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode ಫಲಿತಾಂಶ = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, ದತ್ತಾಂಶ, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, ದತ್ತಾಂಶ, peer, dest, ver);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  infof(ದತ್ತಾಂಶ, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->ಪೋರ್ಟ್);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
    ರಚನೆ http_resp *resp, bool udp_tunnel,
    proxy_inspect_ಫಲಿತಾಂಶ *presult)
{
  ರಚನೆ dynhds_ನಮೂದು *capsule_protocol = ಖಾಲಿ;
  ರಚನೆ dynhds_ನಮೂದು *auth_ಉತ್ತರ = ಖಾಲಿ;
  size_t i, ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(resp);

  ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ = Curl_dynhds_ಎಣಿಕೆ(&resp->ಶೀರ್ಷಿಕೆಗಳು);
  infof(ದತ್ತಾಂಶ, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->ಸ್ಥಿತಿ_2);
  infof(ದತ್ತಾಂಶ, "Response Headers (%zu total):", ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ);
  ಗಾಗಿ(i = 0; i < ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ; i++) {
    ರಚನೆ dynhds_ನಮೂದು *ನಮೂದು = Curl_dynhds_getn(&resp->ಶೀರ್ಷಿಕೆಗಳು, i);
    ಆದರೆ(ನಮೂದು)
      infof(ದತ್ತಾಂಶ, "  %s: %s", ನಮೂದು->name, ನಮೂದು->ಮೌಲ್ಯ);
  }

  ಆದರೆ(resp->ಸ್ಥಿತಿ_2 == 407) {
    auth_ಉತ್ತರ = Curl_dynhds_cget(&resp->ಶೀರ್ಷಿಕೆಗಳು, "Proxy-Authenticate");
  }

  ಆದರೆ(auth_ಉತ್ತರ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_ಉತ್ತರ->ಮೌಲ್ಯ);
    ಫಲಿತಾಂಶ = Curl_http_ಒಳಹರಿವು_auth(ದತ್ತಾಂಶ, resp->ಸ್ಥಿತಿ_2 == 407,
                                  auth_ಉತ್ತರ->ಮೌಲ್ಯ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(ದತ್ತಾಂಶ->req.newurl) {
      curlx_safefree(ದತ್ತಾಂಶ->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
  }

  ಆದರೆ(udp_tunnel) {
    ಆದರೆ(resp->ಸ್ಥಿತಿ_2 / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->ಶೀರ್ಷಿಕೆಗಳು,
                                           "capsule-protocol");
      ಆದರೆ(capsule_protocol) {
        ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(capsule_protocol->ಮೌಲ್ಯ, "?1", 2) &&
           !capsule_protocol->ಮೌಲ್ಯ[2]) {
          infof(ದತ್ತಾಂಶ, "CONNECT-UDP tunnel established, response %d",
                resp->ಸ್ಥಿತಿ_2);
          *presult = PROXY_INSPECT_OK;
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        failf(ದತ್ತಾಂಶ, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->ಸ್ಥಿತಿ_2, capsule_protocol->ಮೌಲ್ಯ);
        *presult = PROXY_INSPECT_FAILED;
        ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(ದತ್ತಾಂಶ, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->ಸ್ಥಿತಿ_2);
        *presult = PROXY_INSPECT_OK;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->ಸ್ಥಿತಿ_2);
      *presult = PROXY_INSPECT_FAILED;
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
  }

  ಆದರೆ(resp->ಸ್ಥಿತಿ_2 / 100 == 2) {
    infof(ದತ್ತಾಂಶ, "CONNECT tunnel established, response %d", resp->ಸ್ಥಿತಿ_2);
    *presult = PROXY_INSPECT_OK;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
}

ಸ್ಥಿರ CURLcode http_proxy_cf_connect(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *ಮುಗಿದ)
{
  ರಚನೆ cf_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ಆದರೆ(cf->ಮುಂದಿನ) {
    ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!ctx->sub_filter_installed) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn = ಖಾಲಿ;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ಆದರೆ(cf->ಮುಂದಿನ) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ);
    }

    ಆದರೆ(alpn)
      infof(ದತ್ತಾಂಶ, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(ದತ್ತಾಂಶ, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      ಆಯ್ಕೆ(ctx->proxytype) {
      ಸ್ಥಿತಿ CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURLPROXY_HTTPS2:
        alpn = "h2";
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURLPROXY_HTTPS3:
        alpn = "h3";
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        alpn = "http/1.1";
        ನಿಲ್ಲಿಸು;
      }
    }

    ಆದರೆ(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "installing subfilter for HTTP/1.0");
      ಫಲಿತಾಂಶ = Curl_cf_h1_proxy_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(alpn, "http/1.1")) {
      ಪೂರ್ಣಾಂಕ httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      ಫಲಿತಾಂಶ = Curl_cf_h1_proxy_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "installing subfilter for HTTP/2");
      ಫಲಿತಾಂಶ = Curl_cf_h2_proxy_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_NGHTTP2 */
#ಆದರೆ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "installing subfilter for HTTP/3");
      ಫಲಿತಾಂಶ = Curl_cf_h3_proxy_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    ಹೋಗು connect_sub;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    ಫಲಿತಾಂಶ = CURLE_OK;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    cf->connected = TRUE;
    *ಮುಗಿದ = TRUE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_http_proxy_query(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_proxy_ctx *ctx = cf->ctx;
  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_HOST_ಪೋರ್ಟ್:
    *pres1 = (ಪೂರ್ಣಾಂಕ)ctx->tunnel_peer->ಪೋರ್ಟ್;
    *((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)pres2) = ctx->tunnel_peer->hostname;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_ALPN_NEGOTIATED: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_https_proxy_ctx_free(ರಚನೆ cf_proxy_ctx *ctx)
{
  ಆದರೆ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ http_proxy_cf_destroy(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_proxy_ctx *ctx = cf->ctx;
  ಆದರೆ(ctx) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

ರಚನೆ Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_ಸಿದ್ಧತೆ,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ Curl_peer *peer,
                                         ರಚನೆ Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ cf_proxy_ctx *ctx = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(!peer || !tunnel_peer)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = ಖಾಲಿ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

ಹೊರಹೋಗು:
  cf_https_proxy_ctx_free(ctx);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  ಆಯ್ಕೆ(proxytype) {
  ಸ್ಥಿತಿ CURLPROXY_HTTPS3:
    ಹಿಂತಿರುಗಿ TRNSPRT_QUIC;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ TRNSPRT_TCP;
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
