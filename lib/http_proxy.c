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
#include "curl_setup.h"

#include "http_proxy.h"

#ถ้า !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#include "curl_trc.h"
#include "http.h"
#include "url.h"
#include "cfilters.h"
#include "cf-h1-proxy.h"
#include "cf-h2-proxy.h"
#include "connect.h"
#include "vauth/vauth.h"
#include "vquic/vquic.h"
#include "curlx/strparse.h"

static CURLcode dynhds_add_custom(sa_nracanaa Curl_easy *data,
                                  bool is_connect, จำนวนเต็ม httpversion,
                                  bool is_udp, sa_nracanaa dynhds *hds)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa curl_slist *h[2];
  sa_nracanaa curl_slist *headers;
  จำนวนเต็ม numlists = 1; /* by default */
  จำนวนเต็ม i;

  ga_nanaa Curl_proxy_use proxy;

  ถ้า(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  มิฉะนั้น ถ้า(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  มิฉะนั้น
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  switch(proxy) {
  pak_sa HEADER_SERVER:
    h[0] = data->set.headers;
    bha_nga;
  pak_sa HEADER_PROXY:
    h[0] = data->set.headers;
    ถ้า(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    bha_nga;
  pak_sa HEADER_CONNECT:
    ถ้า(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    มิฉะนั้น
      h[0] = data->set.headers;
    bha_nga;
  pak_sa HEADER_CONNECT_UDP:
    ถ้า(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    มิฉะนั้น
      h[0] = data->set.headers;
    bha_nga;
  }

  /* loop through one or two lists */
  สำหรับ(i = 0; i < numlists; i++) {
    สำหรับ(headers = h[i]; headers; headers = headers->next) {
      sa_nracanaa Curl_str name;
      const var_na *value = NULL;
      size_t valuelen = 0;
      const var_na *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ถ้า(!curlx_str_cspn(&ptr, &name, ";:")) {
        ถ้า(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ถ้า(*ptr) {
            value = ptr;
            valuelen = strlen(value);
          }
          มิฉะนั้น {
            /* quirk #1, suppress this header */
            continue;
          }
        }
        มิฉะนั้น ถ้า(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ถ้า(!*ptr) {
            /* quirk #2, send an empty header */
            value = "";
            valuelen = 0;
          }
          มิฉะนั้น {
            /* this may be used for something else in the future,
             * ignore this for now */
            continue;
          }
        }
        มิฉะนั้น
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          continue;
      }
      มิฉะนั้น
        /* no name, move on */
        continue;

      DEBUGASSERT(curlx_strlen(&name) && value);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ถ้า(!curlx_strlen(&name))
        continue;
      ถ้า(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      มิฉะนั้น ถ้า(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      มิฉะนั้น ถ้า(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      มิฉะนั้น ถ้า(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      มิฉะนั้น ถ้า((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      มิฉะนั้น ถ้า((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      มิฉะนั้น {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          value, valuelen);
        ถ้า(result)
          คืน result;
      }
    }
  }

  คืน CURLE_OK;
}

sa_nracanaa cf_proxy_ctx {
  sa_nracanaa Curl_peer *peer; /* proxy */
  sa_nracanaa Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

static จำนวนเต็ม proxy_http_ver_major(proxy_http_ver ver)
{
  switch(ver) {
  pak_sa PROXY_HTTP_V1:
    คืน 11;
  pak_sa PROXY_HTTP_V2:
    คืน 20;
  pak_sa PROXY_HTTP_V3:
    คืน 30;
  }
  คืน 0;
}

static CURLcode http_proxy_create_CONNECT(sa_nracanaa httpreq **preq,
                                          sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          sa_nracanaa Curl_peer *peer,
                                          sa_nracanaa Curl_peer *dest,
                                          proxy_http_ver ver)
{
  var_na *authority = NULL;
  const var_na *ua;
  จำนวนเต็ม httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  sa_nracanaa httpreq *req = NULL;

  (void)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  ถ้า(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              NULL, 0, authority, strlen(authority),
                              NULL, 0);
  ถ้า(result)
    goto out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  ถ้า(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ถ้า(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ถ้า(result)
      goto out;
  }

  ถ้า(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    ถ้า(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ถ้า(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ถ้า(result)
      goto out;
  }

  ถ้า(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ถ้า(result)
      goto out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  ถ้า(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  *preq = req;
  คืน result;
}

static CURLcode http_proxy_create_CONNECTUDP(sa_nracanaa httpreq **preq,
                                             sa_nracanaa Curl_cfilter *cf,
                                             sa_nracanaa Curl_easy *data,
                                             sa_nracanaa Curl_peer *peer,
                                             sa_nracanaa Curl_peer *dest,
                                             proxy_http_ver ver)
{
  const var_na *proxy_scheme = "http", *ua;
  จำนวนเต็ม httpversion = proxy_http_ver_major(ver);
  var_na *authority = NULL;
  var_na *path = NULL;
  var_na *encoded_host = NULL;
  sa_nracanaa httpreq *req = NULL;
  CURLcode result;

  ถ้า(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  ถ้า(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  ถ้า(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    const var_na *s = dest->hostname;
    var_na *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ถ้า(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    d = encoded_host;
    ขณะ(*s) {
      ถ้า(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      มิฉะนั้น
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned จำนวนเต็ม)dest->port);
  }
  มิฉะนั้น {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned จำนวนเต็ม)dest->port);
  }

  ถ้า(!path) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  ถ้า(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ถ้า(result)
      goto out;
  }
  มิฉะนั้น ถ้า(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ถ้า(result)
      goto out;
  }
  มิฉะนั้น {
    result = CURLE_FAILED_INIT;
    goto out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  ถ้า(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ถ้า(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ถ้า(result)
      goto out;
  }

  ถ้า(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    ถ้า(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ถ้า(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ถ้า(result)
      goto out;
  }

  ถ้า(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ถ้า(result)
      goto out;
  }

  ถ้า(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    ถ้า(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    ถ้า(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    ถ้า(result)
      goto out;
  }
  มิฉะนั้น {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    ถ้า(result)
      goto out;

    ถ้า(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      ถ้า(result)
        goto out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  ถ้า(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  คืน result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    sa_nracanaa httpreq **preq, sa_nracanaa Curl_cfilter *cf,
    sa_nracanaa Curl_easy *data, sa_nracanaa Curl_peer *peer, sa_nracanaa Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  ถ้า(result)
    คืน result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  คืน CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
    sa_nracanaa http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  sa_nracanaa dynhds_entry *capsule_protocol = NULL;
  sa_nracanaa dynhds_entry *auth_reply = NULL;
  size_t i, header_count;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_count = Curl_dynhds_count(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_count);
  สำหรับ(i = 0; i < header_count; i++) {
    sa_nracanaa dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    ถ้า(entry)
      infof(data, "  %s: %s", entry->name, entry->value);
  }

  ถ้า(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  ถ้า(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->value);
    result = Curl_http_input_auth(data, resp->status == 407,
                                  auth_reply->value);
    ถ้า(result)
      คืน result;
    ถ้า(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      คืน CURLE_OK;
    }
  }

  ถ้า(udp_tunnel) {
    ถ้า(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      ถ้า(capsule_protocol) {
        ถ้า(!strncmp(capsule_protocol->value, "?1", 2) &&
           !capsule_protocol->value[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          คืน CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->value);
        *presult = PROXY_INSPECT_FAILED;
        คืน CURLE_COULDNT_CONNECT;
      }
      มิฉะนั้น {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        คืน CURLE_OK;
      }
    }
    มิฉะนั้น {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      คืน CURLE_COULDNT_CONNECT;
    }
  }

  ถ้า(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    คืน CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  คืน CURLE_COULDNT_CONNECT;
}

static CURLcode http_proxy_cf_connect(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      bool *done)
{
  sa_nracanaa cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ถ้า(cf->connected) {
    *done = TRUE;
    คืน CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ถ้า(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ถ้า(result || !*done)
      คืน result;
  }

  *done = FALSE;
  ถ้า(!ctx->sub_filter_installed) {
    const var_na *alpn = NULL;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ถ้า(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    ถ้า(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    มิฉะนั้น ถ้า(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      switch(ctx->proxytype) {
      pak_sa CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        bha_nga;
      pak_sa CURLPROXY_HTTPS2:
        alpn = "h2";
        bha_nga;
      pak_sa CURLPROXY_HTTPS3:
        alpn = "h3";
        bha_nga;
      default:
        alpn = "http/1.1";
        bha_nga;
      }
    }

    ถ้า(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ถ้า(result)
        goto out;
    }
    มิฉะนั้น ถ้า(!strcmp(alpn, "http/1.1")) {
      จำนวนเต็ม httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ถ้า(result)
        goto out;
    }
#ifdef USE_NGHTTP2
    มิฉะนั้น ถ้า(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ถ้า(result)
        goto out;
    }
#endif /* USE_NGHTTP2 */
#ถ้า defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    มิฉะนั้น ถ้า(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_insert_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ถ้า(result)
        goto out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    มิฉะนั้น {
      failf(data, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      result = CURLE_COULDNT_CONNECT;
      goto out;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    goto connect_sub;
  }
  มิฉะนั้น {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  ถ้า(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  คืน result;
}

static CURLcode cf_http_proxy_query(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    จำนวนเต็ม query, จำนวนเต็ม *pres1, void *pres2)
{
  sa_nracanaa cf_proxy_ctx *ctx = cf->ctx;
  switch(query) {
  pak_sa CF_QUERY_HOST_PORT:
    *pres1 = (จำนวนเต็ม)ctx->tunnel_peer->port;
    *((const var_na **)pres2) = ctx->tunnel_peer->hostname;
    คืน CURLE_OK;
  pak_sa CF_QUERY_ALPN_NEGOTIATED: {
    const var_na **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = NULL;
    คืน CURLE_OK;
  }
  default:
    bha_nga;
  }
  คืน cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static void cf_https_proxy_ctx_free(sa_nracanaa cf_proxy_ctx *ctx)
{
  ถ้า(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

static void http_proxy_cf_destroy(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_proxy_ctx *ctx = cf->ctx;
  ถ้า(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

sa_nracanaa Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                         sa_nracanaa Curl_easy *data,
                                         sa_nracanaa Curl_peer *peer,
                                         sa_nracanaa Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  sa_nracanaa Curl_cfilter *cf;
  sa_nracanaa cf_proxy_ctx *ctx = NULL;
  CURLcode result;

  (void)data;
  ถ้า(!peer || !tunnel_peer)
    คืน CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ถ้า(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ถ้า(result)
    goto out;
  ctx = NULL;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  คืน result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  switch(proxytype) {
  pak_sa CURLPROXY_HTTPS3:
    คืน TRNSPRT_QUIC;
  default:
    คืน TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
