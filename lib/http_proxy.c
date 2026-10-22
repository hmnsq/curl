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

#ຖ້າ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

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
                                  bool is_connect, ຈຳນວນເຕັມ httpversion,
                                  bool is_udp, sa_nracanaa dynhds *hds)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa curl_slist *h[2];
  sa_nracanaa curl_slist *headers;
  ຈຳນວນເຕັມ numlists = 1; /* by default */
  ຈຳນວນເຕັມ i;

  ga_nanaa Curl_proxy_use proxy;

  ຖ້າ(is_connect && !is_udp)
    proxy = HEADER_CONNECT;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(is_connect && is_udp)
    proxy = HEADER_CONNECT_UDP;
  ບໍ່ດັ່ງນັ້ນ
    proxy = conn->bits.origin_is_proxy ? HEADER_PROXY : HEADER_SERVER;

  switch(proxy) {
  pak_sa HEADER_SERVER:
    h[0] = data->set.headers;
    bha_nga;
  pak_sa HEADER_PROXY:
    h[0] = data->set.headers;
    ຖ້າ(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    bha_nga;
  pak_sa HEADER_CONNECT:
    ຖ້າ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ບໍ່ດັ່ງນັ້ນ
      h[0] = data->set.headers;
    bha_nga;
  pak_sa HEADER_CONNECT_UDP:
    ຖ້າ(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    ບໍ່ດັ່ງນັ້ນ
      h[0] = data->set.headers;
    bha_nga;
  }

  /* loop through one or two lists */
  ສຳລັບ(i = 0; i < numlists; i++) {
    ສຳລັບ(headers = h[i]; headers; headers = headers->next) {
      sa_nracanaa Curl_str name;
      const var_na *value = NULL;
      size_t valuelen = 0;
      const var_na *ptr = headers->data;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ຖ້າ(!curlx_str_cspn(&ptr, &name, ";:")) {
        ຖ້າ(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ຖ້າ(*ptr) {
            value = ptr;
            valuelen = strlen(value);
          }
          ບໍ່ດັ່ງນັ້ນ {
            /* quirk #1, suppress this header */
            continue;
          }
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ຖ້າ(!*ptr) {
            /* quirk #2, send an empty header */
            value = "";
            valuelen = 0;
          }
          ບໍ່ດັ່ງນັ້ນ {
            /* this may be used for something else in the future,
             * ignore this for now */
            continue;
          }
        }
        ບໍ່ດັ່ງນັ້ນ
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          continue;
      }
      ບໍ່ດັ່ງນັ້ນ
        /* no name, move on */
        continue;

      DEBUGASSERT(curlx_strlen(&name) && value);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ຖ້າ(!curlx_strlen(&name))
        continue;
      ຖ້າ(data->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(data->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(data->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(data->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      ບໍ່ດັ່ງນັ້ນ ຖ້າ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_to_host(data))
        ;
      ບໍ່ດັ່ງນັ້ນ {
        CURLcode result =
          Curl_dynhds_add(hds, curlx_str(&name), curlx_strlen(&name),
                          value, valuelen);
        ຖ້າ(result)
          ສົ່ງຄືນ result;
      }
    }
  }

  ສົ່ງຄືນ CURLE_OK;
}

sa_nracanaa cf_proxy_ctx {
  sa_nracanaa Curl_peer *peer; /* proxy */
  sa_nracanaa Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  BIT(sub_filter_installed);
};

static ຈຳນວນເຕັມ proxy_http_ver_major(proxy_http_ver ver)
{
  switch(ver) {
  pak_sa PROXY_HTTP_V1:
    ສົ່ງຄືນ 11;
  pak_sa PROXY_HTTP_V2:
    ສົ່ງຄືນ 20;
  pak_sa PROXY_HTTP_V3:
    ສົ່ງຄືນ 30;
  }
  ສົ່ງຄືນ 0;
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
  ຈຳນວນເຕັມ httpversion = proxy_http_ver_major(ver);
  CURLcode result;
  sa_nracanaa httpreq *req = NULL;

  (void)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->port);
  ຖ້າ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              NULL, 0, authority, strlen(authority),
                              NULL, 0);
  ຖ້າ(result)
    goto out;

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  ຖ້າ(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ຖ້າ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ຖ້າ(result)
      goto out;
  }

  ຖ້າ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    ຖ້າ(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ຖ້າ(!Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ຖ້າ(result)
      goto out;
  }

  ຖ້າ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ຖ້າ(result)
      goto out;
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             FALSE, &req->headers);

out:
  ຖ້າ(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  *preq = req;
  ສົ່ງຄືນ result;
}

static CURLcode http_proxy_create_CONNECTUDP(sa_nracanaa httpreq **preq,
                                             sa_nracanaa Curl_cfilter *cf,
                                             sa_nracanaa Curl_easy *data,
                                             sa_nracanaa Curl_peer *peer,
                                             sa_nracanaa Curl_peer *dest,
                                             proxy_http_ver ver)
{
  const var_na *proxy_scheme = "http", *ua;
  ຈຳນວນເຕັມ httpversion = proxy_http_ver_major(ver);
  var_na *authority = NULL;
  var_na *path = NULL;
  var_na *encoded_host = NULL;
  sa_nracanaa httpreq *req = NULL;
  CURLcode result;

  ຖ້າ(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->port);
  ຖ້າ(!authority) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  ຖ້າ(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    const var_na *s = dest->hostname;
    var_na *d;
    size_t hlen = strlen(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ຖ້າ(!encoded_host) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
    d = encoded_host;
    ໃນຂະນະທີ່(*s) {
      ຖ້າ(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      ບໍ່ດັ່ງນັ້ນ
        *d++ = *s;
      s++;
    }
    *d = '\0';
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (unsigned ຈຳນວນເຕັມ)dest->port);
  }
  ບໍ່ດັ່ງນັ້ນ {
    path = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (unsigned ຈຳນວນເຕັມ)dest->port);
  }

  ຖ້າ(!path) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  ຖ້າ(ver == PROXY_HTTP_V1) {
    result = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ຖ້າ(result)
      goto out;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    result = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, strlen(proxy_scheme),
                                authority, strlen(authority),
                                path, strlen(path));
    ຖ້າ(result)
      goto out;
  }
  ບໍ່ດັ່ງນັ້ນ {
    result = CURLE_FAILED_INIT;
    goto out;
  }

  /* Setup the proxy-authorization header, if any */
  result = Curl_http_output_auth(data, cf->conn, req->method, HTTPREQ_GET,
                                 req->authority, NULL, TRUE);
  ຖ້າ(result)
    goto out;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ຖ້າ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Host"))) {
    result = Curl_dynhds_cadd(&req->headers, "Host", authority);
    ຖ້າ(result)
      goto out;
  }

  ຖ້າ(data->req.hd_proxy_auth) {
    result = Curl_dynhds_h1_cadd_line(&req->headers,
                                      data->req.hd_proxy_auth);
    ຖ້າ(result)
      goto out;
  }

  ua = CURL_EASY_STR(data, STRING_USERAGENT);
  ຖ້າ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    result = Curl_dynhds_cadd(&req->headers, "User-Agent", ua);
    ຖ້າ(result)
      goto out;
  }

  ຖ້າ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(data, cf->conn, STRCONST("Proxy-Connection"))) {
    result = Curl_dynhds_cadd(&req->headers, "Proxy-Connection", "Keep-Alive");
    ຖ້າ(result)
      goto out;
  }

  ຖ້າ(ver == PROXY_HTTP_V1) {
    result = Curl_dynhds_cadd(&req->headers, "Connection", "Upgrade");
    ຖ້າ(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Upgrade", "connect-udp");
    ຖ້າ(result)
      goto out;

    result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
    ຖ້າ(result)
      goto out;
  }
  ບໍ່ດັ່ງນັ້ນ {
    result = Curl_dynhds_cadd(&req->headers, ":Protocol", "connect-udp");
    ຖ້າ(result)
      goto out;

    ຖ້າ(ver >= PROXY_HTTP_V2) {
      result = Curl_dynhds_cadd(&req->headers, "Capsule-Protocol", "?1");
      ຖ້າ(result)
        goto out;
    }
  }

  result = dynhds_add_custom(data, TRUE, httpversion,
                             TRUE, &req->headers);

out:
  ຖ້າ(result && req) {
    Curl_http_req_free(req);
    req = NULL;
  }
  curlx_free(authority);
  curlx_free(path);
  curlx_free(encoded_host);
  *preq = req;
  ສົ່ງຄືນ result;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    sa_nracanaa httpreq **preq, sa_nracanaa Curl_cfilter *cf,
    sa_nracanaa Curl_easy *data, sa_nracanaa Curl_peer *peer, sa_nracanaa Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode result = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, data, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, data, peer, dest, ver);
  ຖ້າ(result)
    ສົ່ງຄືນ result;

  infof(data, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->port);
  ສົ່ງຄືນ CURLE_OK;
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
  ສຳລັບ(i = 0; i < header_count; i++) {
    sa_nracanaa dynhds_entry *entry = Curl_dynhds_getn(&resp->headers, i);
    ຖ້າ(entry)
      infof(data, "  %s: %s", entry->name, entry->value);
  }

  ຖ້າ(resp->status == 407) {
    auth_reply = Curl_dynhds_cget(&resp->headers, "Proxy-Authenticate");
  }

  ຖ້າ(auth_reply) {
    CURL_TRC_CF(data, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_reply->value);
    result = Curl_http_input_auth(data, resp->status == 407,
                                  auth_reply->value);
    ຖ້າ(result)
      ສົ່ງຄືນ result;
    ຖ້າ(data->req.newurl) {
      curlx_safefree(data->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      ສົ່ງຄືນ CURLE_OK;
    }
  }

  ຖ້າ(udp_tunnel) {
    ຖ້າ(resp->status / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->headers,
                                           "capsule-protocol");
      ຖ້າ(capsule_protocol) {
        ຖ້າ(!strncmp(capsule_protocol->value, "?1", 2) &&
           !capsule_protocol->value[2]) {
          infof(data, "CONNECT-UDP tunnel established, response %d",
                resp->status);
          *presult = PROXY_INSPECT_OK;
          ສົ່ງຄືນ CURLE_OK;
        }
        failf(data, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->status, capsule_protocol->value);
        *presult = PROXY_INSPECT_FAILED;
        ສົ່ງຄືນ CURLE_COULDNT_CONNECT;
      }
      ບໍ່ດັ່ງນັ້ນ {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(data, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->status);
        *presult = PROXY_INSPECT_OK;
        ສົ່ງຄືນ CURLE_OK;
      }
    }
    ບໍ່ດັ່ງນັ້ນ {
      failf(data, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->status);
      *presult = PROXY_INSPECT_FAILED;
      ສົ່ງຄືນ CURLE_COULDNT_CONNECT;
    }
  }

  ຖ້າ(resp->status / 100 == 2) {
    infof(data, "CONNECT tunnel established, response %d", resp->status);
    *presult = PROXY_INSPECT_OK;
    ສົ່ງຄືນ CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  ສົ່ງຄືນ CURLE_COULDNT_CONNECT;
}

static CURLcode http_proxy_cf_connect(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      bool *done)
{
  sa_nracanaa cf_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ຖ້າ(cf->connected) {
    *done = TRUE;
    ສົ່ງຄືນ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ຖ້າ(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ຖ້າ(result || !*done)
      ສົ່ງຄືນ result;
  }

  *done = FALSE;
  ຖ້າ(!ctx->sub_filter_installed) {
    const var_na *alpn = NULL;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ຖ້າ(cf->next) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    }

    ຖ້າ(alpn)
      infof(data, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(!alpn) {
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

    ຖ້າ(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.0");
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ຖ້າ(result)
        goto out;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(alpn, "http/1.1")) {
      ຈຳນວນເຕັມ httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      result = Curl_cf_h1_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ຖ້າ(result)
        goto out;
    }
#ifdef USE_NGHTTP2
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/2");
      result = Curl_cf_h2_proxy_insert_after(cf, data, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ຖ້າ(result)
        goto out;
    }
#endif /* USE_NGHTTP2 */
#ຖ້າ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(data, cf, "installing subfilter for HTTP/3");
      result = Curl_cf_h3_proxy_insert_after(cf, data, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ຖ້າ(result)
        goto out;
    }
#endif /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    ບໍ່ດັ່ງນັ້ນ {
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
  ບໍ່ດັ່ງນັ້ນ {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    result = CURLE_OK;
  }

out:
  ຖ້າ(!result) {
    cf->connected = TRUE;
    *done = TRUE;
  }
  ສົ່ງຄືນ result;
}

static CURLcode cf_http_proxy_query(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    ຈຳນວນເຕັມ query, ຈຳນວນເຕັມ *pres1, void *pres2)
{
  sa_nracanaa cf_proxy_ctx *ctx = cf->ctx;
  switch(query) {
  pak_sa CF_QUERY_HOST_PORT:
    *pres1 = (ຈຳນວນເຕັມ)ctx->tunnel_peer->port;
    *((const var_na **)pres2) = ctx->tunnel_peer->hostname;
    ສົ່ງຄືນ CURLE_OK;
  pak_sa CF_QUERY_ALPN_NEGOTIATED: {
    const var_na **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = NULL;
    ສົ່ງຄືນ CURLE_OK;
  }
  default:
    bha_nga;
  }
  ສົ່ງຄືນ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static void cf_https_proxy_ctx_free(sa_nracanaa cf_proxy_ctx *ctx)
{
  ຖ້າ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

static void http_proxy_cf_destroy(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_proxy_ctx *ctx = cf->ctx;
  ຖ້າ(ctx) {
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
  ຖ້າ(!peer || !tunnel_peer)
    ສົ່ງຄືນ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ຖ້າ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  result = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ຖ້າ(result)
    goto out;
  ctx = NULL;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  cf_https_proxy_ctx_free(ctx);
  ສົ່ງຄືນ result;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  switch(proxytype) {
  pak_sa CURLPROXY_HTTPS3:
    ສົ່ງຄືນ TRNSPRT_QUIC;
  default:
    ສົ່ງຄືນ TRNSPRT_TCP;
  }
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
