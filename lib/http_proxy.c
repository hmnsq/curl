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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

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

ಸ್ಥಿರ CURLcode dynhds_add_custom(ಸಂರಚನಾ Curl_easy *data,
                                  bool is_connect, ಪೂರ್ಣಾಙ್ಕ httpversion,
                                  bool is_udp, ಸಂರಚನಾ dynhds *hds)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ curl_slist *h[2];
  ಸಂರಚನಾ curl_slist *headers;
  ಪೂರ್ಣಾಙ್ಕ numlists = 1; /* by default */
  ಪೂರ್ಣಾಙ್ಕ i;

  ಗಣನಾ Curl_proxy_use proxy;

  ಯದಿ(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  ಅನ್ಯಥಾ ಯದಿ(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  ಅನ್ಯಥಾ
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  ವಿಕಲ್ಪ(proxy) {
  ಪಕ್ಷ HEADER_SERVER:
    h[0] = data->set.headers;
    ಭಙ್ಗ;
  ಪಕ್ಷ HEADER_PROXY:
    h[0] = data->set.headers;
    ಯದಿ(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ HEADER_CONNECT:
    ಯದಿ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ಅನ್ಯಥಾ
      h[0] = data->set.headers;
    ಭಙ್ಗ;
  ಪಕ್ಷ HEADER_CONNECT_UDP:
    ಯದಿ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ಅನ್ಯಥಾ
      h[0] = data->set.headers;
    ಭಙ್ಗ;
  }

  /* loop through one or two lists */
  ಪ್ರತಿ(i = 0; i < numlists; i++) {
    ಪ್ರತಿ(headers = h[i]; headers; headers = headers->next) {
      ಸಂರಚನಾ Curl_str name;
      ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
      size_t valuelen = 0;
      ಧ್ರುವ ವರ್ಣ *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ಯದಿ(!curlx_str_cspn(&ptr, &name, ";:")) {
        ಯದಿ(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ಯದಿ(*ptr) {
            ಮೌಲ್ಯ = ptr;
            valuelen = strlen(ಮೌಲ್ಯ);
          }
          ಅನ್ಯಥಾ {
            /* quirk #1, suppress this header */
            ಅನುವರ್ತ;
          }
        }
        ಅನ್ಯಥಾ ಯದಿ(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ಯದಿ(!*ptr) {
            /* quirk #2, send an empty header */
            ಮೌಲ್ಯ = "";
            valuelen = 0;
          }
          ಅನ್ಯಥಾ {
            /* this may be used for something else in the future,
             * ignore this for now */
            ಅನುವರ್ತ;
          }
        }
        ಅನ್ಯಥಾ
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ
        /* no name, move on */
        ಅನುವರ್ತ;

      DEBUGASSERT(curlx_strlen(&name) && ಮೌಲ್ಯ);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ಯದಿ(!curlx_strlen(&name))
        ಅನುವರ್ತ;
      ಯದಿ(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ಅನ್ಯಥಾ ಯದಿ(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಅನ್ಯಥಾ ಯದಿ(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ಅನ್ಯಥಾ ಯದಿ(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ಅನ್ಯಥಾ ಯದಿ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      ಅನ್ಯಥಾ ಯದಿ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      ಅನ್ಯಥಾ {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          ಮೌಲ್ಯ, valuelen);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ cf_proxy_ctx {
  ಸಂರಚನಾ Curl_peer *peer; /* proxy */
  ಸಂರಚನಾ Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proxy_http_ver_major(proxy_http_ver ver)
{
  ವಿಕಲ್ಪ(ver) {
  ಪಕ್ಷ PROXY_HTTP_V1:
    ಪ್ರತ್ಯಾಗಮ 11;
  ಪಕ್ಷ PROXY_HTTP_V2:
    ಪ್ರತ್ಯಾಗಮ 20;
  ಪಕ್ಷ PROXY_HTTP_V3:
    ಪ್ರತ್ಯಾಗಮ 30;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ CURLcode http_proxy_create_CONNECT(ಸಂರಚನಾ httpreq **preq,
                                          ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ Curl_peer *peer,
                                          ಸಂರಚನಾ Curl_peer *dest,
                                          proxy_http_ver ver)
{
  ವರ್ಣ *authority = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *ua;
  ಪೂರ್ಣಾಙ್ಕ httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  ಸಂರಚನಾ httpreq *req = ರಿಕ್ತ;

  (ಶೂನ್ಯ)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  ಯದಿ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ರಿಕ್ತ, 0, authority, strlen(authority),
                              ರಿಕ್ತ, 0);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ರಿಕ್ತ, TRUE);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ಯದಿ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_ಸಾಲು(&req->headers,
                                      data->req.hd_proxy_auth);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ಯದಿ(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  ಯದಿ(result && req) {
    Curl_http_req_free(req);
    req = ರಿಕ್ತ;
  }
  curlx_free(authority);
  *preq = req;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode http_proxy_create_CONNECTUDP(ಸಂರಚನಾ httpreq **preq,
                                             ಸಂರಚನಾ Curl_cfilter *cf,
                                             ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ Curl_peer *peer,
                                             ಸಂರಚನಾ Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ಧ್ರುವ ವರ್ಣ *proxy_scheme = "http", *ua;
  ಪೂರ್ಣಾಙ್ಕ httpversion = proxy_http_ver_major(ver);
  ವರ್ಣ *authority = ರಿಕ್ತ;
  ವರ್ಣ *path = ರಿಕ್ತ;
  ವರ್ಣ *encoded_host = ರಿಕ್ತ;
  ಸಂರಚನಾ httpreq *req = ರಿಕ್ತ;
  CURLcode result;

  ಯದಿ(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  ಯದಿ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಯದಿ(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ಧ್ರುವ ವರ್ಣ *s = dest->hostname;
    ವರ್ಣ *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ಯದಿ(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
    d = encoded_host;
    ಯಾವತ್(*s) {
      ಯದಿ(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      ಅನ್ಯಥಾ
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned ಪೂರ್ಣಾಙ್ಕ)dest->port);
  }
  ಅನ್ಯಥಾ {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned ಪೂರ್ಣಾಙ್ಕ)dest->port);
  }

  ಯದಿ(!path) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಯದಿ(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ {
    result = CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ರಿಕ್ತ, TRUE);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ಯದಿ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_ಸಾಲು(&req->headers,
                                      data->req.hd_proxy_auth);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ಯದಿ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    ಯದಿ(result)
      ಗಚ್ಛ out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    ಯದಿ(result)
      ಗಚ್ಛ out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    ಯದಿ(result)
      ಗಚ್ಛ out;

    ಯದಿ(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  ಯದಿ(result && req) {
    Curl_http_req_free(req);
    req = ರಿಕ್ತ;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    ಸಂರಚನಾ httpreq **preq, ಸಂರಚನಾ Curl_cfilter *cf,
    ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_peer *peer, ಸಂರಚನಾ Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
    ಸಂರಚನಾ http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  ಸಂರಚನಾ dynhds_entry *capsule_protocol = ರಿಕ್ತ;
  ಸಂರಚನಾ dynhds_entry *auth_reply = ರಿಕ್ತ;
  size_t i, header_ಎಣಿಕೆ;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_ಎಣಿಕೆ = Curl_dynhds_ಎಣಿಕೆ(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_ಎಣಿಕೆ);
  ಪ್ರತಿ(i = 0; i < header_ಎಣಿಕೆ; i++) {
    ಸಂರಚನಾ dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    ಯದಿ(entry)
      infof(data, "  %s: %s", entry->name, entry->ಮೌಲ್ಯ);
  }

  ಯದಿ(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  ಯದಿ(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->ಮೌಲ್ಯ);
    result = Curl_http_ಒಳಹರಿವು_auth(data, resp->status == 407,
                                  auth_reply->ಮೌಲ್ಯ);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
  }

  ಯದಿ(udp_tunnel) {
    ಯದಿ(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      ಯದಿ(capsule_protocol) {
        ಯದಿ(!strncmp(capsule_protocol->ಮೌಲ್ಯ, "?1", 2) &&
           !capsule_protocol->ಮೌಲ್ಯ[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->ಮೌಲ್ಯ);
        *presult = PROXY_INSPECT_FAILED;
        ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
      }
      ಅನ್ಯಥಾ {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
    }
    ಅನ್ಯಥಾ {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
    }
  }

  ಯದಿ(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
}

ಸ್ಥಿರ CURLcode http_proxy_cf_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      bool *done)
{
  ಸಂರಚನಾ cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ಯದಿ(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  *done = FALSE;
  ಯದಿ(!ctx->sub_filter_installed) {
    ಧ್ರುವ ವರ್ಣ *alpn = ರಿಕ್ತ;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ಯದಿ(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    ಯದಿ(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    ಅನ್ಯಥಾ ಯದಿ(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      ವಿಕಲ್ಪ(ctx->proxytype) {
      ಪಕ್ಷ CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        ಭಙ್ಗ;
      ಪಕ್ಷ CURLPROXY_HTTPS2:
        alpn = "h2";
        ಭಙ್ಗ;
      ಪಕ್ಷ CURLPROXY_HTTPS3:
        alpn = "h3";
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        alpn = "http/1.1";
        ಭಙ್ಗ;
      }
    }

    ಯದಿ(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_ಸೇರಿಸು_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(!strcmp(alpn, "http/1.1")) {
      ಪೂರ್ಣಾಙ್ಕ httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_ಸೇರಿಸು_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
#ifdef USE_NGHTTP2
    ಅನ್ಯಥಾ ಯದಿ(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_ಸೇರಿಸು_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
#endif /* USE_NGHTTP2 */
#ಯದಿ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    ಅನ್ಯಥಾ ಯದಿ(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_ಸೇರಿಸು_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    ಅನ್ಯಥಾ {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      ಗಚ್ಛ out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    ಗಚ್ಛ connect_sub;
  }
  ಅನ್ಯಥಾ {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  ಯದಿ(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_http_proxy_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_proxy_ctx *ctx = cf->ctx;
  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_HOST_PORT:
    *pres1 = (ಪೂರ್ಣಾಙ್ಕ)ctx->tunnel_peer->port;
    *((ಧ್ರುವ ವರ್ಣ **)pres2) = ctx->tunnel_peer->hostname;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_ALPN_NEGOTIATED: {
    ಧ್ರುವ ವರ್ಣ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_https_proxy_ctx_free(ಸಂರಚನಾ cf_proxy_ctx *ctx)
{
  ಯದಿ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ http_proxy_cf_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_proxy_ctx *ctx = cf->ctx;
  ಯದಿ(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

ಸಂರಚನಾ Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                         ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ Curl_peer *peer,
                                         ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ cf_proxy_ctx *ctx = ರಿಕ್ತ;
  CURLcode result;

  (ಶೂನ್ಯ)data;
  ಯದಿ(!peer || !tunnel_peer)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ctx = ರಿಕ್ತ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  ಪ್ರತ್ಯಾಗಮ result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  ವಿಕಲ್ಪ(proxytype) {
  ಪಕ್ಷ CURLPROXY_HTTPS3:
    ಪ್ರತ್ಯಾಗಮ TRNSPRT_QUIC;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
