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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "http_proxy.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY)

#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-h1-proxy.h"
#అన్తర్భావ "cf-h2-proxy.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "curlx/strparse.h"

స్థిర CURLcode dynhds_కూడు_custom(నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool is_connect, పూర్ణాంకం httpversion,
                                  bool is_udp, నిర్మాణం dynhds *hds)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం curl_slist *h[2];
  నిర్మాణం curl_slist *శీర్షికలు;
  పూర్ణాంకం numlists = 1; /* by default */
  పూర్ణాంకం i;

  గణన Curl_proxy_use proxy;

  ఒకవేళ(is_connect && !is_udp)
    proxy = శీర్షిక_CONNECT;
  లేకపోతే ఒకవేళ(is_connect && is_udp)
    proxy = శీర్షిక_CONNECT_UDP;
  లేకపోతే
    proxy = conn->బిట్లు.origin_is_proxy ? శీర్షిక_PROXY : శీర్షిక_SERVER;

  ఎంపిక(proxy) {
  స్థితి శీర్షిక_SERVER:
    h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఆపు;
  స్థితి శీర్షిక_PROXY:
    h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఒకవేళ(దత్తాంశం->సమితి.sep_శీర్షికలు) {
      h[1] = దత్తాంశం->సమితి.proxyheaders;
      numlists++;
    }
    ఆపు;
  స్థితి శీర్షిక_CONNECT:
    ఒకవేళ(దత్తాంశం->సమితి.sep_శీర్షికలు)
      h[0] = దత్తాంశం->సమితి.proxyheaders;
    లేకపోతే
      h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఆపు;
  స్థితి శీర్షిక_CONNECT_UDP:
    ఒకవేళ(దత్తాంశం->సమితి.sep_శీర్షికలు)
      h[0] = దత్తాంశం->సమితి.proxyheaders;
    లేకపోతే
      h[0] = దత్తాంశం->సమితి.శీర్షికలు;
    ఆపు;
  }

  /* loop through one or two lists */
  కొరకు(i = 0; i < numlists; i++) {
    కొరకు(శీర్షికలు = h[i]; శీర్షికలు; శీర్షికలు = శీర్షికలు->తదుపరి) {
      నిర్మాణం Curl_str name;
      స్థిరరాశి అక్షరం *విలువ = ఖాళీ;
      size_t valuelen = 0;
      స్థిరరాశి అక్షరం *ptr = శీర్షికలు->దత్తాంశం;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      ఒకవేళ(!curlx_str_cspn(&ptr, &name, ";:")) {
        ఒకవేళ(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          ఒకవేళ(*ptr) {
            విలువ = ptr;
            valuelen = అక్షరశ్రేణి_పొడవు(విలువ);
          }
          లేకపోతే {
            /* quirk #1, suppress this header */
            కొనసాగు;
          }
        }
        లేకపోతే ఒకవేళ(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          ఒకవేళ(!*ptr) {
            /* quirk #2, send an empty header */
            విలువ = "";
            valuelen = 0;
          }
          లేకపోతే {
            /* this may be used for something else in the future,
             * ignore this for now */
            కొనసాగు;
          }
        }
        లేకపోతే
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          కొనసాగు;
      }
      లేకపోతే
        /* no name, move on */
        కొనసాగు;

      DEBUGASSERT(curlx_strlen(&name) && విలువ);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      ఒకవేళ(!curlx_strlen(&name))
        కొనసాగు;
      ఒకవేళ(దత్తాంశం->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      లేకపోతే ఒకవేళ(దత్తాంశం->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      లేకపోతే ఒకవేళ(దత్తాంశం->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      లేకపోతే ఒకవేళ(దత్తాంశం->req.authneg &&
              /* while doing auth neg, do not allow the custom length since
                 we will force length zero then */
              curlx_str_casecompare(&name, "Content-Length"))
        ;
      లేకపోతే ఒకవేళ((httpversion >= 20) &&
              curlx_str_casecompare(&name, "Transfer-Encoding"))
        ;
      /* HTTP/2 and HTTP/3 do not support chunked requests */
      లేకపోతే ఒకవేళ((curlx_str_casecompare(&name, "Authorization") ||
               curlx_str_casecompare(&name, "Cookie")) &&
              /* be careful of sending this potentially sensitive header to
                 other hosts */
              !Curl_auth_allowed_గమ్యం_host(దత్తాంశం))
        ;
      లేకపోతే {
        CURLcode ఫలితం =
          Curl_dynhds_కూడు(hds, curlx_str(&name), curlx_strlen(&name),
                          విలువ, valuelen);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
    }
  }

  తిరిగి CURLE_OK;
}

నిర్మాణం cf_proxy_ctx {
  నిర్మాణం Curl_peer *peer; /* proxy */
  నిర్మాణం Curl_peer *tunnel_peer; /* tunnel destination */
  uint8_t proxytype;
  uint8_t tunnel_transport;
  బిట్టు(sub_filter_installed);
};

స్థిర పూర్ణాంకం proxy_http_ver_major(proxy_http_ver ver)
{
  ఎంపిక(ver) {
  స్థితి PROXY_HTTP_V1:
    తిరిగి 11;
  స్థితి PROXY_HTTP_V2:
    తిరిగి 20;
  స్థితి PROXY_HTTP_V3:
    తిరిగి 30;
  }
  తిరిగి 0;
}

స్థిర CURLcode http_proxy_create_CONNECT(నిర్మాణం httpreq **preq,
                                          నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం Curl_peer *peer,
                                          నిర్మాణం Curl_peer *dest,
                                          proxy_http_ver ver)
{
  అక్షరం *authority = ఖాళీ;
  స్థిరరాశి అక్షరం *ua;
  పూర్ణాంకం httpversion = proxy_http_ver_major(ver);
  CURLcode ఫలితం;
  నిర్మాణం httpreq *req = ఖాళీ;

  (శూన్య)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->పోర్టు);
  ఒకవేళ(!authority) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              ఖాళీ, 0, authority, అక్షరశ్రేణి_పొడవు(authority),
                              ఖాళీ, 0);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* Setup the proxy-authorization header, if any */
  ఫలితం = Curl_http_output_auth(దత్తాంశం, cf->conn, req->పద్ధతి, HTTPREQ_GET,
                                 req->authority, ఖాళీ, TRUE);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ఒకవేళ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(దత్తాంశం, cf->conn, STRCONST("Host"))) {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Host", authority);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(దత్తాంశం->req.hd_proxy_auth) {
    ఫలితం = Curl_dynhds_h1_cadd_పంక్తి(&req->శీర్షికలు,
                                      దత్తాంశం->req.hd_proxy_auth);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ua = CURL_EASY_STR(దత్తాంశం, STRING_USERAGENT);
  ఒకవేళ(!Curl_checkProxyheaders(దత్తాంశం, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "User-Agent", ua);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(దత్తాంశం, cf->conn, STRCONST("Proxy-Connection"))) {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Proxy-Connection", "Keep-Alive");
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఫలితం = dynhds_కూడు_custom(దత్తాంశం, TRUE, httpversion,
                             FALSE, &req->శీర్షికలు);

బహిర్గతం:
  ఒకవేళ(ఫలితం && req) {
    Curl_http_req_free(req);
    req = ఖాళీ;
  }
  curlx_free(authority);
  *preq = req;
  తిరిగి ఫలితం;
}

స్థిర CURLcode http_proxy_create_CONNECTUDP(నిర్మాణం httpreq **preq,
                                             నిర్మాణం Curl_cfilter *cf,
                                             నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_peer *peer,
                                             నిర్మాణం Curl_peer *dest,
                                             proxy_http_ver ver)
{
  స్థిరరాశి అక్షరం *proxy_scheme = "http", *ua;
  పూర్ణాంకం httpversion = proxy_http_ver_major(ver);
  అక్షరం *authority = ఖాళీ;
  అక్షరం *మార్గం = ఖాళీ;
  అక్షరం *encoded_host = ఖాళీ;
  నిర్మాణం httpreq *req = ఖాళీ;
  CURLcode ఫలితం;

  ఒకవేళ(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->పోర్టు);
  ఒకవేళ(!authority) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    స్థిరరాశి అక్షరం *s = dest->hostname;
    అక్షరం *d;
    size_t hlen = అక్షరశ్రేణి_పొడవు(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    ఒకవేళ(!encoded_host) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
    d = encoded_host;
    అయితే(*s) {
      ఒకవేళ(*s == ':') {
        *d++ = '%';
        *d++ = '3';
        *d++ = 'A';
      }
      లేకపోతే
        *d++ = *s;
      s++;
    }
    *d = '\0';
    మార్గం = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (గుర్తులేని పూర్ణాంకం)dest->పోర్టు);
  }
  లేకపోతే {
    మార్గం = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (గుర్తులేని పూర్ణాంకం)dest->పోర్టు);
  }

  ఒకవేళ(!మార్గం) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ver == PROXY_HTTP_V1) {
    ఫలితం = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, అక్షరశ్రేణి_పొడవు(proxy_scheme),
                                authority, అక్షరశ్రేణి_పొడవు(authority),
                                మార్గం, అక్షరశ్రేణి_పొడవు(మార్గం));
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    ఫలితం = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, అక్షరశ్రేణి_పొడవు(proxy_scheme),
                                authority, అక్షరశ్రేణి_పొడవు(authority),
                                మార్గం, అక్షరశ్రేణి_పొడవు(మార్గం));
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే {
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  /* Setup the proxy-authorization header, if any */
  ఫలితం = Curl_http_output_auth(దత్తాంశం, cf->conn, req->పద్ధతి, HTTPREQ_GET,
                                 req->authority, ఖాళీ, TRUE);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  ఒకవేళ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(దత్తాంశం, cf->conn, STRCONST("Host"))) {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Host", authority);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(దత్తాంశం->req.hd_proxy_auth) {
    ఫలితం = Curl_dynhds_h1_cadd_పంక్తి(&req->శీర్షికలు,
                                      దత్తాంశం->req.hd_proxy_auth);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ua = CURL_EASY_STR(దత్తాంశం, STRING_USERAGENT);
  ఒకవేళ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(దత్తాంశం, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "User-Agent", ua);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(దత్తాంశం, cf->conn, STRCONST("Proxy-Connection"))) {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Proxy-Connection", "Keep-Alive");
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ver == PROXY_HTTP_V1) {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Connection", "Upgrade");
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Upgrade", "connect-udp");
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Capsule-Protocol", "?1");
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే {
    ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, ":Protocol", "connect-udp");
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఒకవేళ(ver >= PROXY_HTTP_V2) {
      ఫలితం = Curl_dynhds_cadd(&req->శీర్షికలు, "Capsule-Protocol", "?1");
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }

  ఫలితం = dynhds_కూడు_custom(దత్తాంశం, TRUE, httpversion,
                             TRUE, &req->శీర్షికలు);

బహిర్గతం:
  ఒకవేళ(ఫలితం && req) {
    Curl_http_req_free(req);
    req = ఖాళీ;
  }
  curlx_free(authority);
  curlx_free(మార్గం);
  curlx_free(encoded_host);
  *preq = req;
  తిరిగి ఫలితం;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    నిర్మాణం httpreq **preq, నిర్మాణం Curl_cfilter *cf,
    నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_peer *peer, నిర్మాణం Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode ఫలితం = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, దత్తాంశం, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, దత్తాంశం, peer, dest, ver);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  infof(దత్తాంశం, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->పోర్టు);
  తిరిగి CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
    నిర్మాణం http_resp *resp, bool udp_tunnel,
    proxy_inspect_ఫలితం *presult)
{
  నిర్మాణం dynhds_నమోదు *capsule_protocol = ఖాళీ;
  నిర్మాణం dynhds_నమోదు *auth_జవాబు = ఖాళీ;
  size_t i, శీర్షిక_లెక్క;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(resp);

  శీర్షిక_లెక్క = Curl_dynhds_లెక్క(&resp->శీర్షికలు);
  infof(దత్తాంశం, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->స్థితి_2);
  infof(దత్తాంశం, "Response Headers (%zu total):", శీర్షిక_లెక్క);
  కొరకు(i = 0; i < శీర్షిక_లెక్క; i++) {
    నిర్మాణం dynhds_నమోదు *నమోదు_2 = Curl_dynhds_getn(&resp->శీర్షికలు, i);
    ఒకవేళ(నమోదు_2)
      infof(దత్తాంశం, "  %s: %s", నమోదు_2->name, నమోదు_2->విలువ);
  }

  ఒకవేళ(resp->స్థితి_2 == 407) {
    auth_జవాబు = Curl_dynhds_cget(&resp->శీర్షికలు, "Proxy-Authenticate");
  }

  ఒకవేళ(auth_జవాబు) {
    CURL_TRC_CF(దత్తాంశం, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_జవాబు->విలువ);
    ఫలితం = Curl_http_ఆదానం_auth(దత్తాంశం, resp->స్థితి_2 == 407,
                                  auth_జవాబు->విలువ);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(దత్తాంశం->req.newurl) {
      curlx_safefree(దత్తాంశం->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      తిరిగి CURLE_OK;
    }
  }

  ఒకవేళ(udp_tunnel) {
    ఒకవేళ(resp->స్థితి_2 / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->శీర్షికలు,
                                           "capsule-protocol");
      ఒకవేళ(capsule_protocol) {
        ఒకవేళ(!అక్షరశ్రేణి_పోలిక(capsule_protocol->విలువ, "?1", 2) &&
           !capsule_protocol->విలువ[2]) {
          infof(దత్తాంశం, "CONNECT-UDP tunnel established, response %d",
                resp->స్థితి_2);
          *presult = PROXY_INSPECT_OK;
          తిరిగి CURLE_OK;
        }
        failf(దత్తాంశం, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->స్థితి_2, capsule_protocol->విలువ);
        *presult = PROXY_INSPECT_FAILED;
        తిరిగి CURLE_COULDNT_CONNECT;
      }
      లేకపోతే {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(దత్తాంశం, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->స్థితి_2);
        *presult = PROXY_INSPECT_OK;
        తిరిగి CURLE_OK;
      }
    }
    లేకపోతే {
      failf(దత్తాంశం, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->స్థితి_2);
      *presult = PROXY_INSPECT_FAILED;
      తిరిగి CURLE_COULDNT_CONNECT;
    }
  }

  ఒకవేళ(resp->స్థితి_2 / 100 == 2) {
    infof(దత్తాంశం, "CONNECT tunnel established, response %d", resp->స్థితి_2);
    *presult = PROXY_INSPECT_OK;
    తిరిగి CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  తిరిగి CURLE_COULDNT_CONNECT;
}

స్థిర CURLcode http_proxy_cf_connect(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *పూర్తి)
{
  నిర్మాణం cf_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  ఒకవేళ(cf->తదుపరి) {
    ఫలితం = cf->తదుపరి->cft->do_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  *పూర్తి = FALSE;
  ఒకవేళ(!ctx->sub_filter_installed) {
    స్థిరరాశి అక్షరం *alpn = ఖాళీ;

    /* in case of h3_proxy, cf->next will be NULL initially */
    ఒకవేళ(cf->తదుపరి) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->తదుపరి, దత్తాంశం);
    }

    ఒకవేళ(alpn)
      infof(దత్తాంశం, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    లేకపోతే ఒకవేళ(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(దత్తాంశం, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
      ఎంపిక(ctx->proxytype) {
      స్థితి CURLPROXY_HTTP_1_0:
        alpn = "http/1.0";
        ఆపు;
      స్థితి CURLPROXY_HTTPS2:
        alpn = "h2";
        ఆపు;
      స్థితి CURLPROXY_HTTPS3:
        alpn = "h3";
        ఆపు;
      అప్రమేయ:
        alpn = "http/1.1";
        ఆపు;
      }
    }

    ఒకవేళ(!strcmp(alpn, "http/1.0")) {
      CURL_TRC_CF(దత్తాంశం, cf, "installing subfilter for HTTP/1.0");
      ఫలితం = Curl_cf_h1_proxy_చొప్పించు_after(cf, దత్తాంశం, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(!strcmp(alpn, "http/1.1")) {
      పూర్ణాంకం httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(దత్తాంశం, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      ఫలితం = Curl_cf_h1_proxy_చొప్పించు_after(cf, దత్తాంశం, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
#నిర్వచితమైతే USE_NGHTTP2
    లేకపోతే ఒకవేళ(!strcmp(alpn, "h2")) {
      CURL_TRC_CF(దత్తాంశం, cf, "installing subfilter for HTTP/2");
      ఫలితం = Curl_cf_h2_proxy_చొప్పించు_after(cf, దత్తాంశం, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
#అయితే_ముగింపు /* USE_NGHTTP2 */
#ఒకవేళ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    లేకపోతే ఒకవేళ(!strcmp(alpn, "h3")) {
      CURL_TRC_CF(దత్తాంశం, cf, "installing subfilter for HTTP/3");
      ఫలితం = Curl_cf_h3_proxy_చొప్పించు_after(cf, దత్తాంశం, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
#అయితే_ముగింపు /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    లేకపోతే {
      failf(దత్తాంశం, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      ఫలితం = CURLE_COULDNT_CONNECT;
      వెళ్ళు బహిర్గతం;
    }

    ctx->sub_filter_installed = TRUE;
    /* after we installed the filter "below" us, we call connect
     * on out sub-chain again.
     */
    వెళ్ళు connect_sub;
  }
  లేకపోతే {
    /* subchain connected and we had already installed the protocol filter.
     * This means the protocol tunnel is established, we are done. */
    DEBUGASSERT(ctx->sub_filter_installed);
    ఫలితం = CURLE_OK;
  }

బహిర్గతం:
  ఒకవేళ(!ఫలితం) {
    cf->connected = TRUE;
    *పూర్తి = TRUE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_http_proxy_query(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_proxy_ctx *ctx = cf->ctx;
  ఎంపిక(query) {
  స్థితి CF_QUERY_HOST_పోర్టు:
    *pres1 = (పూర్ణాంకం)ctx->tunnel_peer->పోర్టు;
    *((స్థిరరాశి అక్షరం **)pres2) = ctx->tunnel_peer->hostname;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_ALPN_NEGOTIATED: {
    స్థిరరాశి అక్షరం **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ఖాళీ;
    తిరిగి CURLE_OK;
  }
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

స్థిర శూన్య cf_https_proxy_ctx_free(నిర్మాణం cf_proxy_ctx *ctx)
{
  ఒకవేళ(ctx) {
    Curl_peer_unlink(&ctx->peer);
    Curl_peer_unlink(&ctx->tunnel_peer);
    curlx_free(ctx);
  }
}

స్థిర శూన్య http_proxy_cf_destroy(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_proxy_ctx *ctx = cf->ctx;
  ఒకవేళ(ctx) {
    CURL_TRC_CF(దత్తాంశం, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

నిర్మాణం Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_అమరిక,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం Curl_peer *peer,
                                         నిర్మాణం Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం cf_proxy_ctx *ctx = ఖాళీ;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  ఒకవేళ(!peer || !tunnel_peer)
    తిరిగి CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ctx = ఖాళీ;
  Curl_conn_cf_చొప్పించు_after(cf_at, cf);

బహిర్గతం:
  cf_https_proxy_ctx_free(ctx);
  తిరిగి ఫలితం;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  ఎంపిక(proxytype) {
  స్థితి CURLPROXY_HTTPS3:
    తిరిగి TRNSPRT_QUIC;
  అప్రమేయ:
    తిరిగి TRNSPRT_TCP;
  }
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
