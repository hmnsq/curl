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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "http_proxy.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-h1-proxy.h"
#અન્તર્ભાવ "cf-h2-proxy.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "curlx/strparse.h"

સ્થિર CURLcode dynhds_add_custom(સંરચના Curl_easy *data,
                                  bool is_connect, પૂર્ણાંક httpversion,
                                  bool is_udp, સંરચના dynhds *hds)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના curl_slist *h[2];
  સંરચના curl_slist *headers;
  પૂર્ણાંક numlists = 1; /* by default */
  પૂર્ણાંક i;

  ગણના Curl_proxy_use proxy;

  જો(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  નહીંતર જો(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  નહીંતર
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  પસંદગી(proxy) {
  સ્થિતિ HEADER_SERVER:
    h[0] = data->set.headers;
    તોડો;
  સ્થિતિ HEADER_PROXY:
    h[0] = data->set.headers;
    જો(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    તોડો;
  સ્થિતિ HEADER_CONNECT:
    જો(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    નહીંતર
      h[0] = data->set.headers;
    તોડો;
  સ્થિતિ HEADER_CONNECT_UDP:
    જો(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    નહીંતર
      h[0] = data->set.headers;
    તોડો;
  }

  /* loop through one or two lists */
  માટે(i = 0; i < numlists; i++) {
    માટે(headers = h[i]; headers; headers = headers->next) {
      સંરચના Curl_str name;
      અચલ અક્ષર *કિંમત = ખાલી;
      size_t valuelen = 0;
      અચલ અક્ષર *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      જો(!curlx_str_cspn(&ptr, &name, ";:")) {
        જો(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          જો(*ptr) {
            કિંમત = ptr;
            valuelen = strlen(કિંમત);
          }
          નહીંતર {
            /* quirk #1, suppress this header */
            ચાલુ;
          }
        }
        નહીંતર જો(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          જો(!*ptr) {
            /* quirk #2, send an empty header */
            કિંમત = "";
            valuelen = 0;
          }
          નહીંતર {
            /* this may be used for something else in the future,
             * ignore this for now */
            ચાલુ;
          }
        }
        નહીંતર
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          ચાલુ;
      }
      નહીંતર
        /* no name, move on */
        ચાલુ;

      DEBUGASSERT(curlx_strlen(&name) && કિંમત);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      જો(!curlx_strlen(&name))
        ચાલુ;
      જો(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      નહીંતર જો(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      નહીંતર જો(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      નહીંતર જો(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      નહીંતર જો((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      નહીંતર જો((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      નહીંતર {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          કિંમત, valuelen);
        જો(result)
          પાછા result;
      }
    }
  }

  પાછા CURLE_OK;
}

સંરચના cf_proxy_ctx {
  સંરચના Curl_peer *peer; /* proxy */
  સંરચના Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

સ્થિર પૂર્ણાંક proxy_http_ver_major(proxy_http_ver ver)
{
  પસંદગી(ver) {
  સ્થિતિ PROXY_HTTP_V1:
    પાછા 11;
  સ્થિતિ PROXY_HTTP_V2:
    પાછા 20;
  સ્થિતિ PROXY_HTTP_V3:
    પાછા 30;
  }
  પાછા 0;
}

સ્થિર CURLcode http_proxy_create_CONNECT(સંરચના httpreq **preq,
                                          સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          સંરચના Curl_peer *peer,
                                          સંરચના Curl_peer *dest,
                                          proxy_http_ver ver)
{
  અક્ષર *authority = ખાલી;
  અચલ અક્ષર *ua;
  પૂર્ણાંક httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  સંરચના httpreq *req = ખાલી;

  (શૂન્ય)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  જો(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ખાલી, 0, authority, strlen(authority),
                              ખાલી, 0);
  જો(result)
    જાઓ out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ખાલી, TRUE);
  જો(result)
    જાઓ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  જો(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    જો(result)
      જાઓ out;
  }

  જો(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_લીટી(&req->headers,
                                      data->req.hd_proxy_auth);
    જો(result)
      જાઓ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  જો(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    જો(result)
      જાઓ out;
  }

  જો(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    જો(result)
      જાઓ out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  જો(result && req) {
    Curl_http_req_free(req);
    req = ખાલી;
  }
  curlx_free(authority);
  *preq = req;
  પાછા result;
}

સ્થિર CURLcode http_proxy_create_CONNECTUDP(સંરચના httpreq **preq,
                                             સંરચના Curl_cfilter *cf,
                                             સંરચના Curl_easy *data,
                                             સંરચના Curl_peer *peer,
                                             સંરચના Curl_peer *dest,
                                             proxy_http_ver ver)
{
  અચલ અક્ષર *proxy_scheme = "http", *ua;
  પૂર્ણાંક httpversion = proxy_http_ver_major(ver);
  અક્ષર *authority = ખાલી;
  અક્ષર *path = ખાલી;
  અક્ષર *encoded_host = ખાલી;
  સંરચના httpreq *req = ખાલી;
  CURLcode result;

  જો(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  જો(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  જો(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    અચલ અક્ષર *s = dest->hostname;
    અક્ષર *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    જો(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
    d = encoded_host;
    જ્યારે(*s) {
      જો(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      નહીંતર
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned પૂર્ણાંક)dest->port);
  }
  નહીંતર {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned પૂર્ણાંક)dest->port);
  }

  જો(!path) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  જો(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    જો(result)
      જાઓ out;
  }
  નહીંતર જો(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    જો(result)
      જાઓ out;
  }
  નહીંતર {
    result = CURLE_FAILED_INIT;
    જાઓ out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, ખાલી, TRUE);
  જો(result)
    જાઓ out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  જો(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    જો(result)
      જાઓ out;
  }

  જો(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_લીટી(&req->headers,
                                      data->req.hd_proxy_auth);
    જો(result)
      જાઓ out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  જો(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    જો(result)
      જાઓ out;
  }

  જો(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    જો(result)
      જાઓ out;
  }

  જો(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    જો(result)
      જાઓ out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    જો(result)
      જાઓ out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    જો(result)
      જાઓ out;
  }
  નહીંતર {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    જો(result)
      જાઓ out;

    જો(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      જો(result)
        જાઓ out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  જો(result && req) {
    Curl_http_req_free(req);
    req = ખાલી;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  પાછા result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    સંરચના httpreq **preq, સંરચના Curl_cfilter *cf,
    સંરચના Curl_easy *data, સંરચના Curl_peer *peer, સંરચના Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  જો(result)
    પાછા result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  પાછા CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
    સંરચના http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  સંરચના dynhds_entry *capsule_protocol = ખાલી;
  સંરચના dynhds_entry *auth_reply = ખાલી;
  size_t i, header_ગણતરી;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_ગણતરી = Curl_dynhds_ગણતરી(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_ગણતરી);
  માટે(i = 0; i < header_ગણતરી; i++) {
    સંરચના dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    જો(entry)
      infof(data, "  %s: %s", entry->name, entry->કિંમત);
  }

  જો(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  જો(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->કિંમત);
    result = Curl_http_આગત_auth(data, resp->status == 407,
                                  auth_reply->કિંમત);
    જો(result)
      પાછા result;
    જો(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      પાછા CURLE_OK;
    }
  }

  જો(udp_tunnel) {
    જો(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      જો(capsule_protocol) {
        જો(!strncmp(capsule_protocol->કિંમત, "?1", 2) &&
           !capsule_protocol->કિંમત[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          પાછા CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->કિંમત);
        *presult = PROXY_INSPECT_FAILED;
        પાછા CURLE_COULDNT_CONNECT;
      }
      નહીંતર {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        પાછા CURLE_OK;
      }
    }
    નહીંતર {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      પાછા CURLE_COULDNT_CONNECT;
    }
  }

  જો(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    પાછા CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  પાછા CURLE_COULDNT_CONNECT;
}

સ્થિર CURLcode http_proxy_cf_connect(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      bool *done)
{
  સંરચના cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  જો(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  *done = FALSE;
  જો(!ctx->sub_filter_installed) {
    અચલ અક્ષર *alpn = ખાલી;

    /* in case of h3_proxy, cf->next will be NULL initially */
    જો(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    જો(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    નહીંતર જો(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      પસંદગી(ctx->proxytype) {
      સ્થિતિ CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        તોડો;
      સ્થિતિ CURLPROXY_HTTPS2:
        alpn = "h2";
        તોડો;
      સ્થિતિ CURLPROXY_HTTPS3:
        alpn = "h3";
        તોડો;
      મૂળભૂત:
        alpn = "http/1.1";
        તોડો;
      }
    }

    જો(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_ઉમેરો_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      જો(result)
        જાઓ out;
    }
    નહીંતર જો(!strcmp(alpn, "http/1.1")) {
      પૂર્ણાંક httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_ઉમેરો_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      જો(result)
        જાઓ out;
    }
#ifdef USE_NGHTTP2
    નહીંતર જો(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_ઉમેરો_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      જો(result)
        જાઓ out;
    }
#endif /* USE_NGHTTP2 */
#જો defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    નહીંતર જો(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_ઉમેરો_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      જો(result)
        જાઓ out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    નહીંતર {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      જાઓ out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    જાઓ connect_sub;
  }
  નહીંતર {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  જો(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  પાછા result;
}

સ્થિર CURLcode cf_http_proxy_query(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_proxy_ctx *ctx = cf->ctx;
  પસંદગી(query) {
  સ્થિતિ CF_QUERY_HOST_PORT:
    *pres1 = (પૂર્ણાંક)ctx->tunnel_peer->port;
    *((અચલ અક્ષર **)pres2) = ctx->tunnel_peer->hostname;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_ALPN_NEGOTIATED: {
    અચલ અક્ષર **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ખાલી;
    પાછા CURLE_OK;
  }
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સ્થિર શૂન્ય cf_https_proxy_ctx_free(સંરચના cf_proxy_ctx *ctx)
{
  જો(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

સ્થિર શૂન્ય http_proxy_cf_destroy(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  સંરચના cf_proxy_ctx *ctx = cf->ctx;
  જો(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

સંરચના Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                         સંરચના Curl_easy *data,
                                         સંરચના Curl_peer *peer,
                                         સંરચના Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  સંરચના Curl_cfilter *cf;
  સંરચના cf_proxy_ctx *ctx = ખાલી;
  CURLcode result;

  (શૂન્ય)data;
  જો(!peer || !tunnel_peer)
    પાછા CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  જો(result)
    જાઓ out;
  ctx = ખાલી;
  Curl_conn_cf_ઉમેરો_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  પાછા result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  પસંદગી(proxytype) {
  સ્થિતિ CURLPROXY_HTTPS3:
    પાછા TRNSPRT_QUIC;
  મૂળભૂત:
    પાછા TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
