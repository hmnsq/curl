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

#nếu !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

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
                                  bool is_connect, số_nguyên httpversion,
                                  bool is_udp, struct dynhds *hds)
{
  struct connectdata *conn = data->conn;
  struct curl_slist *h[2];
  struct curl_slist *headers;
  số_nguyên numlists = 1; /* by default */
  số_nguyên i;

  enum Curl_proxy_use proxy;

  nếu(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  không_thì nếu(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  không_thì
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  switch(proxy) {
  case HEADER_SERVER:
    h[0] = data->set.headers;
    break;
  case HEADER_PROXY:
    h[0] = data->set.headers;
    nếu(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    break;
  case HEADER_CONNECT:
    nếu(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    không_thì
      h[0] = data->set.headers;
    break;
  case HEADER_CONNECT_UDP:
    nếu(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    không_thì
      h[0] = data->set.headers;
    break;
  }

  /* loop through one or two lists */
  cho(i = 0; i < numlists; i++) {
    cho(headers = h[i]; headers; headers = headers->next) {
      struct Curl_str name;
      const char *value = NULL;
      size_t valuelen = 0;
      const char *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      nếu(!curlx_str_cspn(&ptr, &name, ";:")) {
        nếu(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          nếu(*ptr) {
            value = ptr;
            valuelen = strlen(value);
          }
          không_thì {
            /* quirk #1, suppress this header */
            continue;
          }
        }
        không_thì nếu(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          nếu(!*ptr) {
            /* quirk #2, send an empty header */
            value = "";
            valuelen = 0;
          }
          không_thì {
            /* this may be used for something else in the future,
             * ignore this for now */
            continue;
          }
        }
        không_thì
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          continue;
      }
      không_thì
        /* no name, move on */
        continue;

      DEBUGASSERT(curlx_strlen(&name) && value);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      nếu(!curlx_strlen(&name))
        continue;
      nếu(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      không_thì nếu(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      không_thì nếu(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      không_thì nếu(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      không_thì nếu((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      không_thì nếu((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      không_thì {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          value, valuelen);
        nếu(result)
          trả_về result;
      }
    }
  }

  trả_về CURLE_OK;
}

struct cf_proxy_ctx {
  struct Curl_peer *peer; /* proxy */
  struct Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

static số_nguyên proxy_http_ver_major(proxy_http_ver ver)
{
  switch(ver) {
  case PROXY_HTTP_V1:
    trả_về 11;
  case PROXY_HTTP_V2:
    trả_về 20;
  case PROXY_HTTP_V3:
    trả_về 30;
  }
  trả_về 0;
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
  số_nguyên httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  struct httpreq *req = NULL;

  (void)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  nếu(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              NULL, 0, authority, strlen(authority),
                              NULL, 0);
  nếu(result)
    goto out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  nếu(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  nếu(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    nếu(result)
      goto out;
  }

  nếu(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    nếu(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  nếu(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    nếu(result)
      goto out;
  }

  nếu(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    nếu(result)
      goto out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  nếu(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  *preq = req;
  trả_về result;
}

static CURLcode http_proxy_create_CONNECTUDP(struct httpreq **preq,
                                             struct Curl_cfilter *cf,
                                             struct Curl_easy *data,
                                             struct Curl_peer *peer,
                                             struct Curl_peer *dest,
                                             proxy_http_ver ver)
{
  const char *proxy_scheme = "http", *ua;
  số_nguyên httpversion = proxy_http_ver_major(ver);
  char *authority = NULL;
  char *path = NULL;
  char *encoded_host = NULL;
  struct httpreq *req = NULL;
  CURLcode result;

  nếu(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  nếu(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  nếu(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    const char *s = dest->hostname;
    char *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    nếu(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    d = encoded_host;
    trong_khi(*s) {
      nếu(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      không_thì
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned số_nguyên)dest->port);
  }
  không_thì {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned số_nguyên)dest->port);
  }

  nếu(!path) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  nếu(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    nếu(result)
      goto out;
  }
  không_thì nếu(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    nếu(result)
      goto out;
  }
  không_thì {
    result = CURLE_FAILED_INIT;
    goto out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  nếu(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  nếu(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    nếu(result)
      goto out;
  }

  nếu(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    nếu(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  nếu(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    nếu(result)
      goto out;
  }

  nếu(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    nếu(result)
      goto out;
  }

  nếu(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    nếu(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    nếu(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    nếu(result)
      goto out;
  }
  không_thì {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    nếu(result)
      goto out;

    nếu(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      nếu(result)
        goto out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  nếu(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  trả_về result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    struct httpreq **preq, struct Curl_cfilter *cf,
    struct Curl_easy *data, struct Curl_peer *peer, struct Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  nếu(result)
    trả_về result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  trả_về CURLE_OK;
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
  cho(i = 0; i < header_count; i++) {
    struct dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    nếu(entry)
      infof(data, "  %s: %s", entry->name, entry->value);
  }

  nếu(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  nếu(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->value);
    result = Curl_http_input_auth(data, resp->status == 407,
                                  auth_reply->value);
    nếu(result)
      trả_về result;
    nếu(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      trả_về CURLE_OK;
    }
  }

  nếu(udp_tunnel) {
    nếu(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      nếu(capsule_protocol) {
        nếu(!strncmp(capsule_protocol->value, "?1", 2) &&
           !capsule_protocol->value[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          trả_về CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->value);
        *presult = PROXY_INSPECT_FAILED;
        trả_về CURLE_COULDNT_CONNECT;
      }
      không_thì {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        trả_về CURLE_OK;
      }
    }
    không_thì {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      trả_về CURLE_COULDNT_CONNECT;
    }
  }

  nếu(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    trả_về CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  trả_về CURLE_COULDNT_CONNECT;
}

static CURLcode http_proxy_cf_connect(struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      bool *done)
{
  struct cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  nếu(cf->connected) {
    *done = TRUE;
    trả_về CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  nếu(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    nếu(result || !*done)
      trả_về result;
  }

  *done = FALSE;
  nếu(!ctx->sub_filter_installed) {
    const char *alpn = NULL;

    /* in case of h3_proxy, cf->next will be NULL initially */
    nếu(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    nếu(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    không_thì nếu(!alpn) {
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

    nếu(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      nếu(result)
        goto out;
    }
    không_thì nếu(!strcmp(alpn, "http/1.1")) {
      số_nguyên httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      nếu(result)
        goto out;
    }
#ifdef USE_NGHTTP2
    không_thì nếu(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      nếu(result)
        goto out;
    }
#endif /* USE_NGHTTP2 */
#nếu defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    không_thì nếu(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_insert_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      nếu(result)
        goto out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    không_thì {
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
  không_thì {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  nếu(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  trả_về result;
}

static CURLcode cf_http_proxy_query(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    số_nguyên query, số_nguyên *pres1, void *pres2)
{
  struct cf_proxy_ctx *ctx = cf->ctx;
  switch(query) {
  case CF_QUERY_HOST_PORT:
    *pres1 = (số_nguyên)ctx->tunnel_peer->port;
    *((const char **)pres2) = ctx->tunnel_peer->hostname;
    trả_về CURLE_OK;
  case CF_QUERY_ALPN_NEGOTIATED: {
    const char **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = NULL;
    trả_về CURLE_OK;
  }
  default:
    break;
  }
  trả_về cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static void cf_https_proxy_ctx_free(struct cf_proxy_ctx *ctx)
{
  nếu(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

static void http_proxy_cf_destroy(struct Curl_cfilter *cf,
                                  struct Curl_easy *data)
{
  struct cf_proxy_ctx *ctx = cf->ctx;
  nếu(ctx) {
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
  nếu(!peer || !tunnel_peer)
    trả_về CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, sizeof(*ctx));
  nếu(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  nếu(result)
    goto out;
  ctx = NULL;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  trả_về result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  switch(proxytype) {
  case CURLPROXY_HTTPS3:
    trả_về TRNSPRT_QUIC;
  default:
    trả_về TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
