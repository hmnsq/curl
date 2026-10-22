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
#समावेश "curl_setup.h"

#समावेश "http_proxy.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#समावेश "curl_trc.h"
#समावेश "http.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "cf-h1-proxy.h"
#समावेश "cf-h2-proxy.h"
#समावेश "connect.h"
#समावेश "vauth/vauth.h"
#समावेश "vquic/vquic.h"
#समावेश "curlx/strparse.h"

अटल CURLcode dynhds_add_custom(काष्ठा Curl_easy *तथ्य,
                                  bool is_connect, पूर्णांक httpversion,
                                  bool is_udp, काष्ठा dynhds *hds)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा curl_slist *h[2];
  काष्ठा curl_slist *headers;
  पूर्णांक numlists = 1; /* by default */
  पूर्णांक i;

  क्रमागत Curl_proxy_use proxy;

  अगर(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  अन्यथा अगर(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  अन्यथा
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  चयन(proxy) {
  हाल HEADER_SERVER:
    h[0] = तथ्य->set.headers;
    अवरोध;
  हाल HEADER_PROXY:
    h[0] = तथ्य->set.headers;
    अगर(तथ्य->set.sep_headers) {
      h[1] = तथ्य->set.proxyheaders;
      numlists++;
    }
    अवरोध;
  हाल HEADER_CONNECT:
    अगर(तथ्य->set.sep_headers)
      h[0] = तथ्य->set.proxyheaders;
    अन्यथा
      h[0] = तथ्य->set.headers;
    अवरोध;
  हाल HEADER_CONNECT_UDP:
    अगर(तथ्य->set.sep_headers)
      h[0] = तथ्य->set.proxyheaders;
    अन्यथा
      h[0] = तथ्य->set.headers;
    अवरोध;
  }

  /* loop through one or two lists */
  क्रम(i = 0; i < numlists; i++) {
    क्रम(headers = h[i]; headers; headers = headers->अग्र) {
      काष्ठा Curl_str name;
      स्थिर अक्षर *मूल्य = शून्य;
      माप_प्रकार valuelen = 0;
      स्थिर अक्षर *ptr = headers->तथ्य;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      अगर(!curlx_str_cspn(&ptr, &name, ";:")) {
        अगर(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          अगर(*ptr) {
            मूल्य = ptr;
            valuelen = म_माप(मूल्य);
          }
          अन्यथा {
            /* quirk #1, suppress this header */
            जारी;
          }
        }
        अन्यथा अगर(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          अगर(!*ptr) {
            /* quirk #2, send an empty header */
            मूल्य = "";
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

      DEBUGASSERT(curlx_strlen(&name) && मूल्य);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      अगर(!curlx_strlen(&name))
        जारी;
      अगर(तथ्य->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा अगर(तथ्य->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा अगर(तथ्य->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा अगर(तथ्य->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      अन्यथा अगर((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      अन्यथा अगर((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(तथ्य))
        ;
      अन्यथा {
        CURLcode परिणाम =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          मूल्य, valuelen);
        अगर(परिणाम)
          वापस परिणाम;
      }
    }
  }

  वापस CURLE_OK;
}

काष्ठा cf_proxy_ctx {
  काष्ठा Curl_peer *peer; /* proxy */
  काष्ठा Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

अटल पूर्णांक proxy_http_ver_major(proxy_http_ver ver)
{
  चयन(ver) {
  हाल PROXY_HTTP_V1:
    वापस 11;
  हाल PROXY_HTTP_V2:
    वापस 20;
  हाल PROXY_HTTP_V3:
    वापस 30;
  }
  वापस 0;
}

अटल CURLcode http_proxy_create_CONNECT(काष्ठा httpreq **preq,
                                          काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          काष्ठा Curl_peer *peer,
                                          काष्ठा Curl_peer *dest,
                                          proxy_http_ver ver)
{
  अक्षर *authority = शून्य;
  स्थिर अक्षर *ua;
  पूर्णांक httpversion = proxy_http_ver_major(ver);
  CURLcode परिणाम;
  काष्ठा httpreq *req = शून्य;

  (व्योम)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  अगर(!authority) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  परिणाम = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              शून्य, 0, authority, म_माप(authority),
                              शून्य, 0);
  अगर(परिणाम)
    जाओ out;

  /* Setup the proxy-authorization header, if any */
  परिणाम = Curl_http_output_auth(तथ्य, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, शून्य, TRUE);
  अगर(परिणाम)
    जाओ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  अगर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(तथ्य, cf->conn, STRCONST("Host"))) {
    परिणाम = Curl_dynhds_cadd(&req->headers, "Host", authority);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(तथ्य->req.hd_proxy_auth) {
    परिणाम = Curl_dynhds_h1_cadd_पंक्ति(&req->headers,
                                      तथ्य->req.hd_proxy_auth);
    अगर(परिणाम)
      जाओ out;
  }

  ua = CURL_EASY_STR(तथ्य, STRING_USERAGENT);
  अगर(!Curl_checkProxyheaders(तथ्य, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    परिणाम = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(तथ्य, cf->conn, STRCONST("Proxy-Connection"))) {
    परिणाम = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    अगर(परिणाम)
      जाओ out;
  }

  परिणाम = dynhds_add_custom(तथ्य, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  अगर(परिणाम && req) {
    Curl_http_req_free(req);
    req = शून्य;
  }
  curlx_free(authority);
  *preq = req;
  वापस परिणाम;
}

अटल CURLcode http_proxy_create_CONNECTUDP(काष्ठा httpreq **preq,
                                             काष्ठा Curl_cfilter *cf,
                                             काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_peer *peer,
                                             काष्ठा Curl_peer *dest,
                                             proxy_http_ver ver)
{
  स्थिर अक्षर *proxy_scheme = "http", *ua;
  पूर्णांक httpversion = proxy_http_ver_major(ver);
  अक्षर *authority = शून्य;
  अक्षर *path = शून्य;
  अक्षर *encoded_host = शून्य;
  काष्ठा httpreq *req = शून्य;
  CURLcode परिणाम;

  अगर(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  अगर(!authority) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  अगर(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    स्थिर अक्षर *s = dest->hostname;
    अक्षर *d;
    माप_प्रकार hlen = म_माप(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    अगर(!encoded_host) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
    d = encoded_host;
    जबतक(*s) {
      अगर(*s == ':') {
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

  अगर(!path) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  अगर(ver == PROXY_HTTP_V1) {
    परिणाम = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, म_माप(proxy_scheme),
                                authority, म_माप(authority),
                                path, म_माप(path));
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा अगर(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    परिणाम = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, म_माप(proxy_scheme),
                                authority, म_माप(authority),
                                path, म_माप(path));
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा {
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }

  /* Setup the proxy-authorization header, if any */
  परिणाम = Curl_http_output_auth(तथ्य, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, शून्य, TRUE);
  अगर(परिणाम)
    जाओ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  अगर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(तथ्य, cf->conn, STRCONST("Host"))) {
    परिणाम = Curl_dynhds_cadd(&req->headers, "Host", authority);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(तथ्य->req.hd_proxy_auth) {
    परिणाम = Curl_dynhds_h1_cadd_पंक्ति(&req->headers,
                                      तथ्य->req.hd_proxy_auth);
    अगर(परिणाम)
      जाओ out;
  }

  ua = CURL_EASY_STR(तथ्य, STRING_USERAGENT);
  अगर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(तथ्य, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    परिणाम = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(तथ्य, cf->conn, STRCONST("Proxy-Connection"))) {
    परिणाम = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    अगर(परिणाम)
      जाओ out;
  }

  अगर(ver == PROXY_HTTP_V1) {
    परिणाम = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    अगर(परिणाम)
      जाओ out;

    परिणाम = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    अगर(परिणाम)
      जाओ out;

    परिणाम = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा {
    परिणाम = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    अगर(परिणाम)
      जाओ out;

    अगर(ver >= PROXY_HTTP_V2) {
      परिणाम = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      अगर(परिणाम)
        जाओ out;
    }
  }

  परिणाम = dynhds_add_custom(तथ्य, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  अगर(परिणाम && req) {
    Curl_http_req_free(req);
    req = शून्य;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  वापस परिणाम;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    काष्ठा httpreq **preq, काष्ठा Curl_cfilter *cf,
    काष्ठा Curl_easy *तथ्य, काष्ठा Curl_peer *peer, काष्ठा Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode परिणाम = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, तथ्य, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, तथ्य, peer, dest, ver);
  अगर(परिणाम)
    वापस परिणाम;

  infof(तथ्य, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  वापस CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
    काष्ठा http_resp *resp, bool udp_tunnel,
    proxy_inspect_परिणाम *presult)
{
  काष्ठा dynhds_entry *capsule_protocol = शून्य;
  काष्ठा dynhds_entry *auth_reply = शून्य;
  माप_प्रकार i, header_गिनती;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(resp);

  header_गिनती = Curl_dynhds_गिनती(&resp->headers);
  infof(तथ्य, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(तथ्य, "Response Headers (%zu total):", header_गिनती);
  क्रम(i = 0; i < header_गिनती; i++) {
    काष्ठा dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    अगर(entry)
      infof(तथ्य, "  %s: %s", entry->name, entry->मूल्य);
  }

  अगर(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  अगर(auth_reply) {
    CURL_TRC_CF(तथ्य, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->मूल्य);
    परिणाम = Curl_http_निवेश_auth(तथ्य, resp->status == 407,
                                  auth_reply->मूल्य);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(तथ्य->req.newurl) {
      curlx_safefree(तथ्य->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      वापस CURLE_OK;
    }
  }

  अगर(udp_tunnel) {
    अगर(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      अगर(capsule_protocol) {
        अगर(!म_भेदन(capsule_protocol->मूल्य, "?1", 2) &&
           !capsule_protocol->मूल्य[2]) {
          infof(तथ्य, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          वापस CURLE_OK;
        }
        failf(तथ्य, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->मूल्य);
        *presult = PROXY_INSPECT_FAILED;
        वापस CURLE_COULDNT_CONNECT;
      }
      अन्यथा {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(तथ्य, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        वापस CURLE_OK;
      }
    }
    अन्यथा {
      failf(तथ्य, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      वापस CURLE_COULDNT_CONNECT;
    }
  }

  अगर(resp->status / 100 == 2) {
    infof(तथ्य, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    वापस CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  वापस CURLE_COULDNT_CONNECT;
}

अटल CURLcode http_proxy_cf_connect(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      bool *done)
{
  काष्ठा cf_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  CURL_TRC_CF(तथ्य, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  अगर(cf->अग्र) {
    परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  *done = FALSE;
  अगर(!ctx->sub_filter_installed) {
    स्थिर अक्षर *alpn = शून्य;

    /* in case of h3_proxy, cf->next will be NULL initially */
    अगर(cf->अग्र) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->अग्र, तथ्य);
    }

    अगर(alpn)
      infof(तथ्य, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    अन्यथा अगर(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(तथ्य, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      चयन(ctx->proxytype) {
      हाल CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        अवरोध;
      हाल CURLPROXY_HTTPS2:
        alpn = "h2";
        अवरोध;
      हाल CURLPROXY_HTTPS3:
        alpn = "h3";
        अवरोध;
      शेष:
        alpn = "http/1.1";
        अवरोध;
      }
    }

    अगर(!म_भेद(alpn, "http/1.0")) {
      CURL_TRC_CF(तथ्य, cf, "installing subfilter for HTTP/1.0");
      परिणाम = Curl_cf_h1_proxy_जोड़ो_after(cf, तथ्य, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      अगर(परिणाम)
        जाओ out;
    }
    अन्यथा अगर(!म_भेद(alpn, "http/1.1")) {
      पूर्णांक httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(तथ्य, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      परिणाम = Curl_cf_h1_proxy_जोड़ो_after(cf, तथ्य, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      अगर(परिणाम)
        जाओ out;
    }
#अगर_घोषित USE_NGHTTP2
    अन्यथा अगर(!म_भेद(alpn, "h2")) {
      CURL_TRC_CF(तथ्य, cf, "installing subfilter for HTTP/2");
      परिणाम = Curl_cf_h2_proxy_जोड़ो_after(cf, तथ्य, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      अगर(परिणाम)
        जाओ out;
    }
#पूर्ण_अगर /* USE_NGHTTP2 */
#अगर defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    अन्यथा अगर(!म_भेद(alpn, "h3")) {
      CURL_TRC_CF(तथ्य, cf, "installing subfilter for HTTP/3");
      परिणाम = Curl_cf_h3_proxy_जोड़ो_after(cf, तथ्य, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      अगर(परिणाम)
        जाओ out;
    }
#पूर्ण_अगर /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    अन्यथा {
      failf(तथ्य, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      परिणाम = CURLE_COULDNT_CONNECT;
      जाओ out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    जाओ connect_sub;
  }
  अन्यथा {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    परिणाम = CURLE_OK;
  }

out:
  अगर(!परिणाम) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  वापस परिणाम;
}

अटल CURLcode cf_http_proxy_query(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_proxy_ctx *ctx = cf->ctx;
  चयन(query) {
  हाल CF_QUERY_HOST_PORT:
    *pres1 = (पूर्णांक)ctx->tunnel_peer->port;
    *((स्थिर अक्षर **)pres2) = ctx->tunnel_peer->hostname;
    वापस CURLE_OK;
  हाल CF_QUERY_ALPN_NEGOTIATED: {
    स्थिर अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = शून्य;
    वापस CURLE_OK;
  }
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

अटल व्योम cf_https_proxy_ctx_free(काष्ठा cf_proxy_ctx *ctx)
{
  अगर(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

अटल व्योम http_proxy_cf_destroy(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_proxy_ctx *ctx = cf->ctx;
  अगर(ctx) {
    CURL_TRC_CF(तथ्य, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

काष्ठा Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SETUP,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_peer *peer,
                                         काष्ठा Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  काष्ठा Curl_cfilter *cf;
  काष्ठा cf_proxy_ctx *ctx = शून्य;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  अगर(!peer || !tunnel_peer)
    वापस CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  अगर(परिणाम)
    जाओ out;
  ctx = शून्य;
  Curl_conn_cf_जोड़ो_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  वापस परिणाम;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  चयन(proxytype) {
  हाल CURLPROXY_HTTPS3:
    वापस TRNSPRT_QUIC;
  शेष:
    वापस TRNSPRT_TCP;
  }
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
