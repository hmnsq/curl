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

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

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
                                  bool is_connect, पूर्णांक httpversion,
                                  bool is_udp, संरचना dynhds *hds)
{
  संरचना connectdata *conn = data->conn;
  संरचना curl_slist *h[2];
  संरचना curl_slist *headers;
  पूर्णांक numlists = 1; /* by default */
  पूर्णांक i;

  गणना Curl_proxy_use proxy;

  जर(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  अन्यथा जर(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  अन्यथा
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  निवड(proxy) {
  स्थिती HEADER_SERVER:
    h[0] = data->set.headers;
    थांबा;
  स्थिती HEADER_PROXY:
    h[0] = data->set.headers;
    जर(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    थांबा;
  स्थिती HEADER_CONNECT:
    जर(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    थांबा;
  स्थिती HEADER_CONNECT_UDP:
    जर(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    थांबा;
  }

  /* loop through one or two lists */
  साठी(i = 0; i < numlists; i++) {
    साठी(headers = h[i]; headers; headers = headers->next) {
      संरचना Curl_str name;
      स्थिरांक अक्षर *मूल्य = रिक्त;
      माप_प्रकार valuelen = 0;
      स्थिरांक अक्षर *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      जर(!curlx_str_cspn(&ptr, &name, ";:")) {
        जर(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          जर(*ptr) {
            मूल्य = ptr;
            valuelen = म_माप(मूल्य);
          }
          अन्यथा {
            /* quirk #1, suppress this header */
            सुरूठेवा;
          }
        }
        अन्यथा जर(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          जर(!*ptr) {
            /* quirk #2, send an empty header */
            मूल्य = "";
            valuelen = 0;
          }
          अन्यथा {
            /* this may be used for something else in the future,
             * ignore this for now */
            सुरूठेवा;
          }
        }
        अन्यथा
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          सुरूठेवा;
      }
      अन्यथा
        /* no name, move on */
        सुरूठेवा;

      DEBUGASSERT(curlx_strlen(&name) && मूल्य);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      जर(!curlx_strlen(&name))
        सुरूठेवा;
      जर(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा जर(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा जर(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा जर(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      अन्यथा जर((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      अन्यथा जर((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      अन्यथा {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          मूल्य, valuelen);
        जर(result)
          परत result;
      }
    }
  }

  परत CURLE_OK;
}

संरचना cf_proxy_ctx {
  संरचना Curl_peer *peer; /* proxy */
  संरचना Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

स्थिर पूर्णांक proxy_http_ver_major(proxy_http_ver ver)
{
  निवड(ver) {
  स्थिती PROXY_HTTP_V1:
    परत 11;
  स्थिती PROXY_HTTP_V2:
    परत 20;
  स्थिती PROXY_HTTP_V3:
    परत 30;
  }
  परत 0;
}

स्थिर CURLcode http_proxy_create_CONNECT(संरचना httpreq **preq,
                                          संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          संरचना Curl_peer *peer,
                                          संरचना Curl_peer *dest,
                                          proxy_http_ver ver)
{
  अक्षर *authority = रिक्त;
  स्थिरांक अक्षर *ua;
  पूर्णांक httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  संरचना httpreq *req = रिक्त;

  (शून्य)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  जर(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              रिक्त, 0, authority, म_माप(authority),
                              रिक्त, 0);
  जर(result)
    जा out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  जर(result)
    जा out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  जर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    जर(result)
      जा out;
  }

  जर(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_ओळ(&req->headers,
                                      data->req.hd_proxy_auth);
    जर(result)
      जा out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  जर(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    जर(result)
      जा out;
  }

  जर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    जर(result)
      जा out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  जर(result && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  *preq = req;
  परत result;
}

स्थिर CURLcode http_proxy_create_CONNECTUDP(संरचना httpreq **preq,
                                             संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data,
                                             संरचना Curl_peer *peer,
                                             संरचना Curl_peer *dest,
                                             proxy_http_ver ver)
{
  स्थिरांक अक्षर *proxy_scheme = "http", *ua;
  पूर्णांक httpversion = proxy_http_ver_major(ver);
  अक्षर *authority = रिक्त;
  अक्षर *path = रिक्त;
  अक्षर *encoded_host = रिक्त;
  संरचना httpreq *req = रिक्त;
  CURLcode result;

  जर(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  जर(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  जर(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    स्थिरांक अक्षर *s = dest->hostname;
    अक्षर *d;
    माप_प्रकार hlen = म_माप(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    जर(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      जा out;
    }
    d = encoded_host;
    पर्यंत(*s) {
      जर(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      अन्यथा
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (अचिन्हित पूर्णांक)dest->port);
  }
  अन्यथा {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (अचिन्हित पूर्णांक)dest->port);
  }

  जर(!path) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  जर(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, म_माप(proxy_scheme),
                                authority, म_माप(authority),
                                path, म_माप(path));
    जर(result)
      जा out;
  }
  अन्यथा जर(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, म_माप(proxy_scheme),
                                authority, म_माप(authority),
                                path, म_माप(path));
    जर(result)
      जा out;
  }
  अन्यथा {
    result = CURLE_FAILED_INIT;
    जा out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  जर(result)
    जा out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  जर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    जर(result)
      जा out;
  }

  जर(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_ओळ(&req->headers,
                                      data->req.hd_proxy_auth);
    जर(result)
      जा out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  जर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    जर(result)
      जा out;
  }

  जर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    जर(result)
      जा out;
  }

  जर(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    जर(result)
      जा out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    जर(result)
      जा out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    जर(result)
      जा out;
  }
  अन्यथा {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    जर(result)
      जा out;

    जर(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      जर(result)
        जा out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  जर(result && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  परत result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    संरचना httpreq **preq, संरचना Curl_cfilter *cf,
    संरचना Curl_easy *data, संरचना Curl_peer *peer, संरचना Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  जर(result)
    परत result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  परत CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
    संरचना http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  संरचना dynhds_entry *capsule_protocol = रिक्त;
  संरचना dynhds_entry *auth_reply = रिक्त;
  माप_प्रकार i, header_मोजणी;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_मोजणी = Curl_dynhds_मोजणी(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_मोजणी);
  साठी(i = 0; i < header_मोजणी; i++) {
    संरचना dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    जर(entry)
      infof(data, "  %s: %s", entry->name, entry->मूल्य);
  }

  जर(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  जर(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->मूल्य);
    result = Curl_http_आदान_auth(data, resp->status == 407,
                                  auth_reply->मूल्य);
    जर(result)
      परत result;
    जर(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      परत CURLE_OK;
    }
  }

  जर(udp_tunnel) {
    जर(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      जर(capsule_protocol) {
        जर(!म_भेदन(capsule_protocol->मूल्य, "?1", 2) &&
           !capsule_protocol->मूल्य[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          परत CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->मूल्य);
        *presult = PROXY_INSPECT_FAILED;
        परत CURLE_COULDNT_CONNECT;
      }
      अन्यथा {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        परत CURLE_OK;
      }
    }
    अन्यथा {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      परत CURLE_COULDNT_CONNECT;
    }
  }

  जर(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    परत CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  परत CURLE_COULDNT_CONNECT;
}

स्थिर CURLcode http_proxy_cf_connect(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *done)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  जर(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    जर(result || !*done)
      परत result;
  }

  *done = FALSE;
  जर(!ctx->sub_filter_installed) {
    स्थिरांक अक्षर *alpn = रिक्त;

    /* in case of h3_proxy, cf->next will be NULL initially */
    जर(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    जर(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    अन्यथा जर(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      निवड(ctx->proxytype) {
      स्थिती CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        थांबा;
      स्थिती CURLPROXY_HTTPS2:
        alpn = "h2";
        थांबा;
      स्थिती CURLPROXY_HTTPS3:
        alpn = "h3";
        थांबा;
      पूर्वनिर्धारित:
        alpn = "http/1.1";
        थांबा;
      }
    }

    जर(!म_भेद(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_घाला_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      जर(result)
        जा out;
    }
    अन्यथा जर(!म_भेद(alpn, "http/1.1")) {
      पूर्णांक httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_घाला_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      जर(result)
        जा out;
    }
#ifdef USE_NGHTTP2
    अन्यथा जर(!म_भेद(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_घाला_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      जर(result)
        जा out;
    }
#endif /* USE_NGHTTP2 */
#जर defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    अन्यथा जर(!म_भेद(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_घाला_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      जर(result)
        जा out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    अन्यथा {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      जा out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    जा connect_sub;
  }
  अन्यथा {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  जर(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  परत result;
}

स्थिर CURLcode cf_http_proxy_query(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  निवड(query) {
  स्थिती CF_QUERY_HOST_PORT:
    *pres1 = (पूर्णांक)ctx->tunnel_peer->port;
    *((स्थिरांक अक्षर **)pres2) = ctx->tunnel_peer->hostname;
    परत CURLE_OK;
  स्थिती CF_QUERY_ALPN_NEGOTIATED: {
    स्थिरांक अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    परत CURLE_OK;
  }
  पूर्वनिर्धारित:
    थांबा;
  }
  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर शून्य cf_https_proxy_ctx_free(संरचना cf_proxy_ctx *ctx)
{
  जर(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

स्थिर शून्य http_proxy_cf_destroy(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  जर(ctx) {
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

CURLcode Curl_cf_http_proxy_घाला_after(संरचना Curl_cfilter *cf_at,
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
  जर(!peer || !tunnel_peer)
    परत CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  जर(result)
    जा out;
  ctx = रिक्त;
  Curl_conn_cf_घाला_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  परत result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  निवड(proxytype) {
  स्थिती CURLPROXY_HTTPS3:
    परत TRNSPRT_QUIC;
  पूर्वनिर्धारित:
    परत TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
