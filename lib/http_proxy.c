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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "http_proxy.h"

#सचे !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-h1-proxy.h"
#अन्तर्भाव "cf-h2-proxy.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "curlx/strparse.h"

स्थिर CURLcode dynhds_add_custom(संरचना Curl_easy *data,
                                  bool is_connect, पूर्णाङ्क httpversion,
                                  bool is_udp, संरचना dynhds *hds)
{
  संरचना connectdata *conn = data->conn;
  संरचना curl_slist *h[2];
  संरचना curl_slist *headers;
  पूर्णाङ्क numlists = 1; /* by default */
  पूर्णाङ्क i;

  गणना Curl_proxy_use proxy;

  सचे(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  अञ्ञथा सचे(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  अञ्ञथा
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  विकल्प(proxy) {
  पक्ष HEADER_SERVER:
    h[0] = data->set.headers;
    भङ्ग;
  पक्ष HEADER_PROXY:
    h[0] = data->set.headers;
    सचे(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    भङ्ग;
  पक्ष HEADER_CONNECT:
    सचे(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अञ्ञथा
      h[0] = data->set.headers;
    भङ्ग;
  पक्ष HEADER_CONNECT_UDP:
    सचे(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अञ्ञथा
      h[0] = data->set.headers;
    भङ्ग;
  }

  /* loop through one or two lists */
  अत्थाय(i = 0; i < numlists; i++) {
    अत्थाय(headers = h[i]; headers; headers = headers->next) {
      संरचना Curl_str name;
      ध्रुव वर्ण *मानम् = रिक्त;
      size_t valuelen = 0;
      ध्रुव वर्ण *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      सचे(!curlx_str_cspn(&ptr, &name, ";:")) {
        सचे(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          सचे(*ptr) {
            मानम् = ptr;
            valuelen = strlen(मानम्);
          }
          अञ्ञथा {
            /* quirk #1, suppress this header */
            अनुवर्त;
          }
        }
        अञ्ञथा सचे(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          सचे(!*ptr) {
            /* quirk #2, send an empty header */
            मानम् = "";
            valuelen = 0;
          }
          अञ्ञथा {
            /* this may be used for something else in the future,
             * ignore this for now */
            अनुवर्त;
          }
        }
        अञ्ञथा
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          अनुवर्त;
      }
      अञ्ञथा
        /* no name, move on */
        अनुवर्त;

      DEBUGASSERT(curlx_strlen(&name) && मानम्);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      सचे(!curlx_strlen(&name))
        अनुवर्त;
      सचे(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अञ्ञथा सचे(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अञ्ञथा सचे(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अञ्ञथा सचे(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      अञ्ञथा सचे((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      अञ्ञथा सचे((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      अञ्ञथा {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          मानम्, valuelen);
        सचे(result)
          पटिनिवत्त result;
      }
    }
  }

  पटिनिवत्त CURLE_OK;
}

संरचना cf_proxy_ctx {
  संरचना Curl_peer *peer; /* proxy */
  संरचना Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

स्थिर पूर्णाङ्क proxy_http_ver_major(proxy_http_ver ver)
{
  विकल्प(ver) {
  पक्ष PROXY_HTTP_V1:
    पटिनिवत्त 11;
  पक्ष PROXY_HTTP_V2:
    पटिनिवत्त 20;
  पक्ष PROXY_HTTP_V3:
    पटिनिवत्त 30;
  }
  पटिनिवत्त 0;
}

स्थिर CURLcode http_proxy_create_CONNECT(संरचना httpreq **preq,
                                          संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          संरचना Curl_peer *peer,
                                          संरचना Curl_peer *dest,
                                          proxy_http_ver ver)
{
  वर्ण *authority = रिक्त;
  ध्रुव वर्ण *ua;
  पूर्णाङ्क httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  संरचना httpreq *req = रिक्त;

  (शून्य)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  सचे(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              रिक्त, 0, authority, strlen(authority),
                              रिक्त, 0);
  सचे(result)
    गच्छ out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  सचे(result)
    गच्छ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  सचे(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    सचे(result)
      गच्छ out;
  }

  सचे(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_पङ्क्तिः(&req->headers,
                                      data->req.hd_proxy_auth);
    सचे(result)
      गच्छ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  सचे(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    सचे(result)
      गच्छ out;
  }

  सचे(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    सचे(result)
      गच्छ out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  सचे(result && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  *preq = req;
  पटिनिवत्त result;
}

स्थिर CURLcode http_proxy_create_CONNECTUDP(संरचना httpreq **preq,
                                             संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data,
                                             संरचना Curl_peer *peer,
                                             संरचना Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ध्रुव वर्ण *proxy_scheme = "http", *ua;
  पूर्णाङ्क httpversion = proxy_http_ver_major(ver);
  वर्ण *authority = रिक्त;
  वर्ण *path = रिक्त;
  वर्ण *encoded_host = रिक्त;
  संरचना httpreq *req = रिक्त;
  CURLcode result;

  सचे(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  सचे(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  सचे(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ध्रुव वर्ण *s = dest->hostname;
    वर्ण *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    सचे(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    d = encoded_host;
    याव(*s) {
      सचे(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      अञ्ञथा
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned पूर्णाङ्क)dest->port);
  }
  अञ्ञथा {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned पूर्णाङ्क)dest->port);
  }

  सचे(!path) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  सचे(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    सचे(result)
      गच्छ out;
  }
  अञ्ञथा सचे(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    सचे(result)
      गच्छ out;
  }
  अञ्ञथा {
    result = CURLE_FAILED_INIT;
    गच्छ out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  सचे(result)
    गच्छ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  सचे(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    सचे(result)
      गच्छ out;
  }

  सचे(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_पङ्क्तिः(&req->headers,
                                      data->req.hd_proxy_auth);
    सचे(result)
      गच्छ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  सचे(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    सचे(result)
      गच्छ out;
  }

  सचे(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    सचे(result)
      गच्छ out;
  }

  सचे(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    सचे(result)
      गच्छ out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    सचे(result)
      गच्छ out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    सचे(result)
      गच्छ out;
  }
  अञ्ञथा {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    सचे(result)
      गच्छ out;

    सचे(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      सचे(result)
        गच्छ out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  सचे(result && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  पटिनिवत्त result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    संरचना httpreq **preq, संरचना Curl_cfilter *cf,
    संरचना Curl_easy *data, संरचना Curl_peer *peer, संरचना Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  सचे(result)
    पटिनिवत्त result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
    संरचना http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  संरचना dynhds_entry *capsule_protocol = रिक्त;
  संरचना dynhds_entry *auth_reply = रिक्त;
  size_t i, header_गणनम्;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_गणनम् = Curl_dynhds_गणनम्(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_गणनम्);
  अत्थाय(i = 0; i < header_गणनम्; i++) {
    संरचना dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    सचे(entry)
      infof(data, "  %s: %s", entry->name, entry->मानम्);
  }

  सचे(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  सचे(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->मानम्);
    result = Curl_http_निवेशः_auth(data, resp->status == 407,
                                  auth_reply->मानम्);
    सचे(result)
      पटिनिवत्त result;
    सचे(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      पटिनिवत्त CURLE_OK;
    }
  }

  सचे(udp_tunnel) {
    सचे(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      सचे(capsule_protocol) {
        सचे(!strncmp(capsule_protocol->मानम्, "?1", 2) &&
           !capsule_protocol->मानम्[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          पटिनिवत्त CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->मानम्);
        *presult = PROXY_INSPECT_FAILED;
        पटिनिवत्त CURLE_COULDNT_CONNECT;
      }
      अञ्ञथा {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        पटिनिवत्त CURLE_OK;
      }
    }
    अञ्ञथा {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }
  }

  सचे(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    पटिनिवत्त CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  पटिनिवत्त CURLE_COULDNT_CONNECT;
}

स्थिर CURLcode http_proxy_cf_connect(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *done)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  सचे(cf->connected) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  सचे(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    सचे(result || !*done)
      पटिनिवत्त result;
  }

  *done = FALSE;
  सचे(!ctx->sub_filter_installed) {
    ध्रुव वर्ण *alpn = रिक्त;

    /* in case of h3_proxy, cf->next will be NULL initially */
    सचे(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    सचे(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    अञ्ञथा सचे(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      विकल्प(ctx->proxytype) {
      पक्ष CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        भङ्ग;
      पक्ष CURLPROXY_HTTPS2:
        alpn = "h2";
        भङ्ग;
      पक्ष CURLPROXY_HTTPS3:
        alpn = "h3";
        भङ्ग;
      स्वभाव:
        alpn = "http/1.1";
        भङ्ग;
      }
    }

    सचे(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_निवेशय_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      सचे(result)
        गच्छ out;
    }
    अञ्ञथा सचे(!strcmp(alpn, "http/1.1")) {
      पूर्णाङ्क httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_निवेशय_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      सचे(result)
        गच्छ out;
    }
#ifdef USE_NGHTTP2
    अञ्ञथा सचे(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_निवेशय_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      सचे(result)
        गच्छ out;
    }
#endif /* USE_NGHTTP2 */
#सचे defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    अञ्ञथा सचे(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_निवेशय_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      सचे(result)
        गच्छ out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    अञ्ञथा {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      गच्छ out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    गच्छ connect_sub;
  }
  अञ्ञथा {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  सचे(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode cf_http_proxy_query(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  विकल्प(query) {
  पक्ष CF_QUERY_HOST_PORT:
    *pres1 = (पूर्णाङ्क)ctx->tunnel_peer->port;
    *((ध्रुव वर्ण **)pres2) = ctx->tunnel_peer->hostname;
    पटिनिवत्त CURLE_OK;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    पटिनिवत्त CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  पटिनिवत्त cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर शून्य cf_https_proxy_ctx_free(संरचना cf_proxy_ctx *ctx)
{
  सचे(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

स्थिर शून्य http_proxy_cf_destroy(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  सचे(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

संरचना Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                         संरचना Curl_easy *data,
                                         संरचना Curl_peer *peer,
                                         संरचना Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_proxy_ctx *ctx = रिक्त;
  CURLcode result;

  (शून्य)data;
  सचे(!peer || !tunnel_peer)
    पटिनिवत्त CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  सचे(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  सचे(result)
    गच्छ out;
  ctx = रिक्त;
  Curl_conn_cf_निवेशय_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  पटिनिवत्त result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  विकल्प(proxytype) {
  पक्ष CURLPROXY_HTTPS3:
    पटिनिवत्त TRNSPRT_QUIC;
  स्वभाव:
    पटिनिवत्त TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
