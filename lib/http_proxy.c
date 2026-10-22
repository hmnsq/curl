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

#如果 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

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

static CURLcode dynhds_add_custom(struct Curl_easy *data,
                                  bool is_connect, 整数 httpversion,
                                  bool is_udp, struct dynhds *hds)
{
  struct connectdata *conn = data->conn;
  struct curl_slist *h[2];
  struct curl_slist *headers;
  整数 numlists = 1; /* by default */
  整数 i;

  enum Curl_proxy_use proxy;

  如果(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  否则 如果(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  否则
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  switch(proxy) {
  case HEADER_SERVER:
    h[0] = data->set.headers;
    break;
  case HEADER_PROXY:
    h[0] = data->set.headers;
    如果(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    break;
  case HEADER_CONNECT:
    如果(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    否则
      h[0] = data->set.headers;
    break;
  case HEADER_CONNECT_UDP:
    如果(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    否则
      h[0] = data->set.headers;
    break;
  }

  /* loop through one or two lists */
  为(i = 0; i < numlists; i++) {
    为(headers = h[i]; headers; headers = headers->next) {
      struct Curl_str name;
      const char *value = NULL;
      size_t valuelen = 0;
      const char *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      如果(!curlx_str_cspn(&ptr, &name, ";:")) {
        如果(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          如果(*ptr) {
            value = ptr;
            valuelen = strlen(value);
          }
          否则 {
            /* quirk #1, suppress this header */
            continue;
          }
        }
        否则 如果(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          如果(!*ptr) {
            /* quirk #2, send an empty header */
            value = "";
            valuelen = 0;
          }
          否则 {
            /* this may be used for something else in the future,
             * ignore this for now */
            continue;
          }
        }
        否则
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          continue;
      }
      否则
        /* no name, move on */
        continue;

      DEBUGASSERT(curlx_strlen(&name) && value);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      如果(!curlx_strlen(&name))
        continue;
      如果(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      否则 如果(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      否则 如果(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      否则 如果(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      否则 如果((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      否则 如果((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      否则 {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          value, valuelen);
        如果(result)
          返回 result;
      }
    }
  }

  返回 CURLE_OK;
}

struct cf_proxy_ctx {
  struct Curl_peer *peer; /* proxy */
  struct Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

static 整数 proxy_http_ver_major(proxy_http_ver ver)
{
  switch(ver) {
  case PROXY_HTTP_V1:
    返回 11;
  case PROXY_HTTP_V2:
    返回 20;
  case PROXY_HTTP_V3:
    返回 30;
  }
  返回 0;
}

static CURLcode http_proxy_create_CONNECT(struct httpreq **preq,
                                          struct Curl_cfilter *cf,
                                          struct Curl_easy *data,
                                          struct Curl_peer *peer,
                                          struct Curl_peer *dest,
                                          proxy_http_ver ver)
{
  char *authority = NULL;
  const char *ua;
  整数 httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  struct httpreq *req = NULL;

  (void)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  如果(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              NULL, 0, authority, strlen(authority),
                              NULL, 0);
  如果(result)
    goto out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  如果(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  如果(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    如果(result)
      goto out;
  }

  如果(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    如果(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  如果(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    如果(result)
      goto out;
  }

  如果(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    如果(result)
      goto out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  如果(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  *preq = req;
  返回 result;
}

static CURLcode http_proxy_create_CONNECTUDP(struct httpreq **preq,
                                             struct Curl_cfilter *cf,
                                             struct Curl_easy *data,
                                             struct Curl_peer *peer,
                                             struct Curl_peer *dest,
                                             proxy_http_ver ver)
{
  const char *proxy_scheme = "http", *ua;
  整数 httpversion = proxy_http_ver_major(ver);
  char *authority = NULL;
  char *path = NULL;
  char *encoded_host = NULL;
  struct httpreq *req = NULL;
  CURLcode result;

  如果(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  如果(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  如果(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    const char *s = dest->hostname;
    char *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    如果(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    d = encoded_host;
    当(*s) {
      如果(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      否则
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned 整数)dest->port);
  }
  否则 {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned 整数)dest->port);
  }

  如果(!path) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  如果(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    如果(result)
      goto out;
  }
  否则 如果(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    如果(result)
      goto out;
  }
  否则 {
    result = CURLE_FAILED_INIT;
    goto out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  如果(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  如果(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    如果(result)
      goto out;
  }

  如果(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    如果(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  如果(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    如果(result)
      goto out;
  }

  如果(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    如果(result)
      goto out;
  }

  如果(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    如果(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    如果(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    如果(result)
      goto out;
  }
  否则 {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    如果(result)
      goto out;

    如果(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      如果(result)
        goto out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  如果(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  返回 result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    struct httpreq **preq, struct Curl_cfilter *cf,
    struct Curl_easy *data, struct Curl_peer *peer, struct Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  如果(result)
    返回 result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  返回 CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    struct Curl_cfilter *cf, struct Curl_easy *data,
    struct http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult)
{
  struct dynhds_entry *capsule_protocol = NULL;
  struct dynhds_entry *auth_reply = NULL;
  size_t i, header_count;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(resp);

  header_count = Curl_dynhds_count(&resp->headers);
  infof(data, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->status);
  infof(data, "Response Headers (%zu total):", header_count);
  为(i = 0; i < header_count; i++) {
    struct dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    如果(entry)
      infof(data, "  %s: %s", entry->name, entry->value);
  }

  如果(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  如果(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->value);
    result = Curl_http_input_auth(data, resp->status == 407,
                                  auth_reply->value);
    如果(result)
      返回 result;
    如果(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      返回 CURLE_OK;
    }
  }

  如果(udp_tunnel) {
    如果(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      如果(capsule_protocol) {
        如果(!strncmp(capsule_protocol->value, "?1", 2) &&
           !capsule_protocol->value[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          返回 CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->value);
        *presult = PROXY_INSPECT_FAILED;
        返回 CURLE_COULDNT_CONNECT;
      }
      否则 {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        返回 CURLE_OK;
      }
    }
    否则 {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      返回 CURLE_COULDNT_CONNECT;
    }
  }

  如果(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    返回 CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  返回 CURLE_COULDNT_CONNECT;
}

static CURLcode http_proxy_cf_connect(struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      bool *done)
{
  struct cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  如果(cf->connected) {
    *done = TRUE;
    返回 CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  如果(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    如果(result || !*done)
      返回 result;
  }

  *done = FALSE;
  如果(!ctx->sub_filter_installed) {
    const char *alpn = NULL;

    /* in case of h3_proxy, cf->next will be NULL initially */
    如果(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    如果(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    否则 如果(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(data, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      switch(ctx->proxytype) {
      case CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        break;
      case CURLPROXY_HTTPS2:
        alpn = "h2";
        break;
      case CURLPROXY_HTTPS3:
        alpn = "h3";
        break;
      default:
        alpn = "http/1.1";
        break;
      }
    }

    如果(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      如果(result)
        goto out;
    }
    否则 如果(!strcmp(alpn, "http/1.1")) {
      整数 httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      如果(result)
        goto out;
    }
#ifdef USE_NGHTTP2
    否则 如果(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      如果(result)
        goto out;
    }
#endif /* USE_NGHTTP2 */
#如果 defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    否则 如果(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_insert_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      如果(result)
        goto out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    否则 {
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
  否则 {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  如果(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  返回 result;
}

static CURLcode cf_http_proxy_query(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    整数 query, 整数 *pres1, void *pres2)
{
  struct cf_proxy_ctx *ctx = cf->ctx;
  switch(query) {
  case CF_QUERY_HOST_PORT:
    *pres1 = (整数)ctx->tunnel_peer->port;
    *((const char **)pres2) = ctx->tunnel_peer->hostname;
    返回 CURLE_OK;
  case CF_QUERY_ALPN_NEGOTIATED: {
    const char **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = NULL;
    返回 CURLE_OK;
  }
  default:
    break;
  }
  返回 cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static void cf_https_proxy_ctx_free(struct cf_proxy_ctx *ctx)
{
  如果(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

static void http_proxy_cf_destroy(struct Curl_cfilter *cf,
                                  struct Curl_easy *data)
{
  struct cf_proxy_ctx *ctx = cf->ctx;
  如果(ctx) {
    CURL_TRC_CF(data, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

struct Curl_cftype Curl_cft_http_proxy = {
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

CURLcode Curl_cf_http_proxy_insert_after(struct Curl_cfilter *cf_at,
                                         struct Curl_easy *data,
                                         struct Curl_peer *peer,
                                         struct Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  struct Curl_cfilter *cf;
  struct cf_proxy_ctx *ctx = NULL;
  CURLcode result;

  (void)data;
  如果(!peer || !tunnel_peer)
    返回 CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, sizeof(*ctx));
  如果(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  如果(result)
    goto out;
  ctx = NULL;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  返回 result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  switch(proxytype) {
  case CURLPROXY_HTTPS3:
    返回 TRNSPRT_QUIC;
  default:
    返回 TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
