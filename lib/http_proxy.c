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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

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

  यदि(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  अन्यथा यदि(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  अन्यथा
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  विकल्प(proxy) {
  पक्ष HEADER_SERVER:
    h[0] = data->set.headers;
    रोक;
  पक्ष HEADER_PROXY:
    h[0] = data->set.headers;
    यदि(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    रोक;
  पक्ष HEADER_CONNECT:
    यदि(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    रोक;
  पक्ष HEADER_CONNECT_UDP:
    यदि(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    अन्यथा
      h[0] = data->set.headers;
    रोक;
  }

  /* loop through one or two lists */
  लागि(i = 0; i < numlists; i++) {
    लागि(headers = h[i]; headers; headers = headers->next) {
      संरचना Curl_str name;
      अचर वर्ण *मान = रिक्त;
      माप_प्रकार valuelen = 0;
      अचर वर्ण *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      यदि(!curlx_str_cspn(&ptr, &name, ";:")) {
        यदि(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          यदि(*ptr) {
            मान = ptr;
            valuelen = म_माप(मान);
          }
          अन्यथा {
            /* quirk #1, suppress this header */
            जारी;
          }
        }
        अन्यथा यदि(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          यदि(!*ptr) {
            /* quirk #2, send an empty header */
            मान = "";
            valuelen = 0;
          }
          अन्यथा {
            /* this may be used for something else in the future,
             * ignore this for now */
            जारी;
          }
        }
        अन्यथा
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          जारी;
      }
      अन्यथा
        /* no name, move on */
        जारी;

      DEBUGASSERT(curlx_strlen(&name) && मान);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      यदि(!curlx_strlen(&name))
        जारी;
      यदि(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा यदि(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      अन्यथा यदि((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      अन्यथा यदि((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      अन्यथा {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          मान, valuelen);
        यदि(result)
          फर्काऊ result;
      }
    }
  }

  फर्काऊ CURLE_OK;
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
    फर्काऊ 11;
  पक्ष PROXY_HTTP_V2:
    फर्काऊ 20;
  पक्ष PROXY_HTTP_V3:
    फर्काऊ 30;
  }
  फर्काऊ 0;
}

स्थिर CURLcode http_proxy_create_CONNECT(संरचना httpreq **preq,
                                          संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          संरचना Curl_peer *peer,
                                          संरचना Curl_peer *dest,
                                          proxy_http_ver ver)
{
  वर्ण *authority = रिक्त;
  अचर वर्ण *ua;
  पूर्णाङ्क httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  संरचना httpreq *req = रिक्त;

  (शून्य)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  यदि(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              रिक्त, 0, authority, म_माप(authority),
                              रिक्त, 0);
  यदि(result)
    गच्छ out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  यदि(result)
    गच्छ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    यदि(result)
      गच्छ out;
  }

  यदि(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_पङ्क्ति(&req->headers,
                                      data->req.hd_proxy_auth);
    यदि(result)
      गच्छ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  यदि(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    यदि(result)
      गच्छ out;
  }

  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    यदि(result)
      गच्छ out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  यदि(result && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  *preq = req;
  फर्काऊ result;
}

स्थिर CURLcode http_proxy_create_CONNECTUDP(संरचना httpreq **preq,
                                             संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data,
                                             संरचना Curl_peer *peer,
                                             संरचना Curl_peer *dest,
                                             proxy_http_ver ver)
{
  अचर वर्ण *proxy_scheme = "http", *ua;
  पूर्णाङ्क httpversion = proxy_http_ver_major(ver);
  वर्ण *authority = रिक्त;
  वर्ण *path = रिक्त;
  वर्ण *encoded_host = रिक्त;
  संरचना httpreq *req = रिक्त;
  CURLcode result;

  यदि(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  यदि(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  यदि(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    अचर वर्ण *s = dest->hostname;
    वर्ण *d;
    माप_प्रकार hlen = म_माप(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    यदि(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
    d = encoded_host;
    जबसम्म(*s) {
      यदि(*s == ':') {
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
                         encoded_host, (अचिन्हित पूर्णाङ्क)dest->port);
  }
  अन्यथा {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (अचिन्हित पूर्णाङ्क)dest->port);
  }

  यदि(!path) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  यदि(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, म_माप(proxy_scheme),
                                authority, म_माप(authority),
                                path, म_माप(path));
    यदि(result)
      गच्छ out;
  }
  अन्यथा यदि(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, म_माप(proxy_scheme),
                                authority, म_माप(authority),
                                path, म_माप(path));
    यदि(result)
      गच्छ out;
  }
  अन्यथा {
    result = CURLE_FAILED_INIT;
    गच्छ out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  यदि(result)
    गच्छ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    यदि(result)
      गच्छ out;
  }

  यदि(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_पङ्क्ति(&req->headers,
                                      data->req.hd_proxy_auth);
    यदि(result)
      गच्छ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    यदि(result)
      गच्छ out;
  }

  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    यदि(result)
      गच्छ out;
  }

  यदि(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    यदि(result)
      गच्छ out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    यदि(result)
      गच्छ out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    यदि(result)
      गच्छ out;
  }
  अन्यथा {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    यदि(result)
      गच्छ out;

    यदि(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      यदि(result)
        गच्छ out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  यदि(result && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  फर्काऊ result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    संरचना httpreq **preq, संरचना Curl_cfilter *cf,
    संरचना Curl_easy *data, संरचना Curl_peer *peer, संरचना Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  यदि(result)
    फर्काऊ result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  फर्काऊ CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
    संरचना http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  संरचना dynhds_entry *capsule_protocol = रिक्त;
  संरचना dynhds_entry *auth_reply = रिक्त;
  माप_प्रकार i, header_गन्ती;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_गन्ती = Curl_dynhds_गन्ती(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_गन्ती);
  लागि(i = 0; i < header_गन्ती; i++) {
    संरचना dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    यदि(entry)
      infof(data, "  %s: %s", entry->name, entry->मान);
  }

  यदि(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  यदि(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->मान);
    result = Curl_http_आगत_auth(data, resp->status == 407,
                                  auth_reply->मान);
    यदि(result)
      फर्काऊ result;
    यदि(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      फर्काऊ CURLE_OK;
    }
  }

  यदि(udp_tunnel) {
    यदि(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      यदि(capsule_protocol) {
        यदि(!म_भेदन(capsule_protocol->मान, "?1", 2) &&
           !capsule_protocol->मान[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          फर्काऊ CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->मान);
        *presult = PROXY_INSPECT_FAILED;
        फर्काऊ CURLE_COULDNT_CONNECT;
      }
      अन्यथा {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        फर्काऊ CURLE_OK;
      }
    }
    अन्यथा {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      फर्काऊ CURLE_COULDNT_CONNECT;
    }
  }

  यदि(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    फर्काऊ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  फर्काऊ CURLE_COULDNT_CONNECT;
}

स्थिर CURLcode http_proxy_cf_connect(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *done)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  यदि(cf->connected) {
    *done = TRUE;
    फर्काऊ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  यदि(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    यदि(result || !*done)
      फर्काऊ result;
  }

  *done = FALSE;
  यदि(!ctx->sub_filter_installed) {
    अचर वर्ण *alpn = रिक्त;

    /* in case of h3_proxy, cf->next will be NULL initially */
    यदि(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    यदि(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    अन्यथा यदि(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      विकल्प(ctx->proxytype) {
      पक्ष CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        रोक;
      पक्ष CURLPROXY_HTTPS2:
        alpn = "h2";
        रोक;
      पक्ष CURLPROXY_HTTPS3:
        alpn = "h3";
        रोक;
      स्वभाव:
        alpn = "http/1.1";
        रोक;
      }
    }

    यदि(!म_भेद(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_घुसाउ_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      यदि(result)
        गच्छ out;
    }
    अन्यथा यदि(!म_भेद(alpn, "http/1.1")) {
      पूर्णाङ्क httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_घुसाउ_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      यदि(result)
        गच्छ out;
    }
#ifdef USE_NGHTTP2
    अन्यथा यदि(!म_भेद(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_घुसाउ_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      यदि(result)
        गच्छ out;
    }
#endif /* USE_NGHTTP2 */
#यदि defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    अन्यथा यदि(!म_भेद(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_घुसाउ_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      यदि(result)
        गच्छ out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    अन्यथा {
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
  अन्यथा {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  यदि(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  फर्काऊ result;
}

स्थिर CURLcode cf_http_proxy_query(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  विकल्प(query) {
  पक्ष CF_QUERY_HOST_PORT:
    *pres1 = (पूर्णाङ्क)ctx->tunnel_peer->port;
    *((अचर वर्ण **)pres2) = ctx->tunnel_peer->hostname;
    फर्काऊ CURLE_OK;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    अचर वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    फर्काऊ CURLE_OK;
  }
  स्वभाव:
    रोक;
  }
  फर्काऊ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर शून्य cf_https_proxy_ctx_free(संरचना cf_proxy_ctx *ctx)
{
  यदि(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

स्थिर शून्य http_proxy_cf_destroy(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  यदि(ctx) {
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

CURLcode Curl_cf_http_proxy_घुसाउ_after(संरचना Curl_cfilter *cf_at,
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
  यदि(!peer || !tunnel_peer)
    फर्काऊ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  यदि(result)
    गच्छ out;
  ctx = रिक्त;
  Curl_conn_cf_घुसाउ_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  फर्काऊ result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  विकल्प(proxytype) {
  पक्ष CURLPROXY_HTTPS3:
    फर्काऊ TRNSPRT_QUIC;
  स्वभाव:
    फर्काऊ TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
