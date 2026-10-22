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

स्थिर CURLcode dynhds_सङ्कलय_custom(संरचना Curl_easy *दत्त,
                                  bool is_connect, पूर्णाङ्क httpversion,
                                  bool is_udp, संरचना dynhds *hds)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना curl_slist *h[2];
  संरचना curl_slist *शीर्षकाणि;
  पूर्णाङ्क numlists = 1; /* by default */
  पूर्णाङ्क i;

  गणना Curl_proxy_use proxy;

  यदि(is_connect && !is_udp)
    proxy = शीर्षकम्_CONNECT;
  अन्यथा यदि(is_connect && is_udp)
    proxy = शीर्षकम्_CONNECT_UDP;
  अन्यथा
    proxy = conn->द्वयङ्काः.origin_is_proxy ? शीर्षकम्_PROXY : शीर्षकम्_SERVER;

  विकल्प(proxy) {
  पक्ष शीर्षकम्_SERVER:
    h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    भङ्ग;
  पक्ष शीर्षकम्_PROXY:
    h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    यदि(दत्त->समुच्चयः.sep_शीर्षकाणि) {
      h[1] = दत्त->समुच्चयः.proxyheaders;
      numlists++;
    }
    भङ्ग;
  पक्ष शीर्षकम्_CONNECT:
    यदि(दत्त->समुच्चयः.sep_शीर्षकाणि)
      h[0] = दत्त->समुच्चयः.proxyheaders;
    अन्यथा
      h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    भङ्ग;
  पक्ष शीर्षकम्_CONNECT_UDP:
    यदि(दत्त->समुच्चयः.sep_शीर्षकाणि)
      h[0] = दत्त->समुच्चयः.proxyheaders;
    अन्यथा
      h[0] = दत्त->समुच्चयः.शीर्षकाणि;
    भङ्ग;
  }

  /* loop through one or two lists */
  प्रति(i = 0; i < numlists; i++) {
    प्रति(शीर्षकाणि = h[i]; शीर्षकाणि; शीर्षकाणि = शीर्षकाणि->अग्र) {
      संरचना Curl_str name;
      ध्रुव वर्ण *मानम् = रिक्त;
      size_t valuelen = 0;
      ध्रुव वर्ण *ptr = शीर्षकाणि->दत्त;

      /* There are 2 quirks in place for custom headers:
       * 1. setting only 'name:' to suppress a header from being sent
       * 2. setting only 'name;' to send an empty (illegal) header
       */
      यदि(!curlx_str_cspn(&ptr, &name, ";:")) {
        यदि(!curlx_str_single(&ptr, ':')) {
          curlx_str_passblanks(&ptr);
          यदि(*ptr) {
            मानम् = ptr;
            valuelen = वाक्यदैर्घ्यम्(मानम्);
          }
          अन्यथा {
            /* quirk #1, suppress this header */
            अनुवर्त;
          }
        }
        अन्यथा यदि(!curlx_str_single(&ptr, ';')) {
          curlx_str_passblanks(&ptr);
          यदि(!*ptr) {
            /* quirk #2, send an empty header */
            मानम् = "";
            valuelen = 0;
          }
          अन्यथा {
            /* this may be used for something else in the future,
             * ignore this for now */
            अनुवर्त;
          }
        }
        अन्यथा
          /* neither : nor ; in provided header value. We ignore this
           * silently */
          अनुवर्त;
      }
      अन्यथा
        /* no name, move on */
        अनुवर्त;

      DEBUGASSERT(curlx_strlen(&name) && मानम्);
      /* trim surrounding whitespace so a padded field name (e.g.
         `Authorization :`) cannot slip past the Authorization/Cookie check */
      curlx_str_trimblanks(&name);
      यदि(!curlx_strlen(&name))
        अनुवर्त;
      यदि(दत्त->state.http_host &&
         /* a Host: header was sent already, do not pass on any custom Host:
            header as that will produce *two* in the same request! */
         curlx_str_casecompare(&name, "Host"))
        ;
      अन्यथा यदि(दत्त->state.httpreq == HTTPREQ_POST_FORM &&
              /* this header (extended by formdata.c) is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(दत्त->state.httpreq == HTTPREQ_POST_MIME &&
              /* this header is sent later */
              curlx_str_casecompare(&name, "Content-Type"))
        ;
      अन्यथा यदि(दत्त->req.authneg &&
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
              !Curl_auth_allowed_गन्तव्यम्_host(दत्त))
        ;
      अन्यथा {
        CURLcode परिणाम =
          Curl_dynhds_सङ्कलय(hds, curlx_str(&name), curlx_strlen(&name),
                          मानम्, valuelen);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
    }
  }

  प्रत्यागम CURLE_OK;
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
    प्रत्यागम 11;
  पक्ष PROXY_HTTP_V2:
    प्रत्यागम 20;
  पक्ष PROXY_HTTP_V3:
    प्रत्यागम 30;
  }
  प्रत्यागम 0;
}

स्थिर CURLcode http_proxy_create_CONNECT(संरचना httpreq **preq,
                                          संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त,
                                          संरचना Curl_peer *peer,
                                          संरचना Curl_peer *dest,
                                          proxy_http_ver ver)
{
  वर्ण *authority = रिक्त;
  ध्रुव वर्ण *ua;
  पूर्णाङ्क httpversion = proxy_http_ver_major(ver);
  CURLcode परिणाम;
  संरचना httpreq *req = रिक्त;

  (शून्य)peer;
  authority = curl_maprintf("%s%s%s:%u",
                            dest->ipv6 ? "[" : "",
                            dest->hostname,
                            dest->ipv6 ? "]" : "",
                            dest->पत्तनम्);
  यदि(!authority) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_http_req_make(&req, STRCONST("CONNECT"),
                              रिक्त, 0, authority, वाक्यदैर्घ्यम्(authority),
                              रिक्त, 0);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* Setup the proxy-authorization header, if any */
  परिणाम = Curl_http_output_auth(दत्त, cf->conn, req->विधानम्, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(दत्त, cf->conn, STRCONST("Host"))) {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Host", authority);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(दत्त->req.hd_proxy_auth) {
    परिणाम = Curl_dynhds_h1_cadd_पङ्क्तिः(&req->शीर्षकाणि,
                                      दत्त->req.hd_proxy_auth);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  ua = CURL_EASY_STR(दत्त, STRING_USERAGENT);
  यदि(!Curl_checkProxyheaders(दत्त, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "User-Agent", ua);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(दत्त, cf->conn, STRCONST("Proxy-Connection"))) {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Proxy-Connection", "Keep-Alive");
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  परिणाम = dynhds_सङ्कलय_custom(दत्त, TRUE, httpversion,
                             FALSE, &req->शीर्षकाणि);

बहिर्गमनम्:
  यदि(परिणाम && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  *preq = req;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode http_proxy_create_CONNECTUDP(संरचना httpreq **preq,
                                             संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *दत्त,
                                             संरचना Curl_peer *peer,
                                             संरचना Curl_peer *dest,
                                             proxy_http_ver ver)
{
  ध्रुव वर्ण *proxy_scheme = "http", *ua;
  पूर्णाङ्क httpversion = proxy_http_ver_major(ver);
  वर्ण *authority = रिक्त;
  वर्ण *मार्गः = रिक्त;
  वर्ण *encoded_host = रिक्त;
  संरचना httpreq *req = रिक्त;
  CURLcode परिणाम;

  यदि(cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS2 ||
     cf->conn->http_proxy.proxytype == CURLPROXY_HTTPS3)
    proxy_scheme = "https";

  authority = curl_maprintf("%s%s%s:%d",
                            peer->ipv6 ? "[" : "",
                            peer->hostname,
                            peer->ipv6 ? "]" : "",
                            peer->पत्तनम्);
  यदि(!authority) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  यदि(dest->ipv6) {
    /* RFC 9298: colons in IPv6 addresses MUST be percent-encoded
     * in the URI template (e.g. "2001:db8::1" -> "2001%3Adb8%3A%3A1") */
    ध्रुव वर्ण *s = dest->hostname;
    वर्ण *d;
    size_t hlen = वाक्यदैर्घ्यम्(s);
    encoded_host = curlx_malloc(hlen * 3 + 1);
    यदि(!encoded_host) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
    d = encoded_host;
    यावत्(*s) {
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
    मार्गः = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         encoded_host, (अचिह्नित पूर्णाङ्क)dest->पत्तनम्);
  }
  अन्यथा {
    मार्गः = curl_maprintf("/.well-known/masque/udp/%s/%u/",
                         dest->hostname, (अचिह्नित पूर्णाङ्क)dest->पत्तनम्);
  }

  यदि(!मार्गः) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  यदि(ver == PROXY_HTTP_V1) {
    परिणाम = Curl_http_req_make(&req, "GET", CURL_CSTRLEN("GET"),
                                proxy_scheme, वाक्यदैर्घ्यम्(proxy_scheme),
                                authority, वाक्यदैर्घ्यम्(authority),
                                मार्गः, वाक्यदैर्घ्यम्(मार्गः));
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(ver == PROXY_HTTP_V2 || ver == PROXY_HTTP_V3) {
    परिणाम = Curl_http_req_make(&req, STRCONST("CONNECT"),
                                proxy_scheme, वाक्यदैर्घ्यम्(proxy_scheme),
                                authority, वाक्यदैर्घ्यम्(authority),
                                मार्गः, वाक्यदैर्घ्यम्(मार्गः));
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा {
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  /* Setup the proxy-authorization header, if any */
  परिणाम = Curl_http_output_auth(दत्त, cf->conn, req->विधानम्, HTTPREQ_GET,
                                 req->authority, रिक्त, TRUE);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* If user is not overriding Host: header, we add for HTTP/1.x */
  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(दत्त, cf->conn, STRCONST("Host"))) {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Host", authority);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(दत्त->req.hd_proxy_auth) {
    परिणाम = Curl_dynhds_h1_cadd_पङ्क्तिः(&req->शीर्षकाणि,
                                      दत्त->req.hd_proxy_auth);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  ua = CURL_EASY_STR(दत्त, STRING_USERAGENT);
  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(दत्त, cf->conn, STRCONST("User-Agent")) &&
     ua && *ua) {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "User-Agent", ua);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(ver == PROXY_HTTP_V1 &&
     !Curl_checkProxyheaders(दत्त, cf->conn, STRCONST("Proxy-Connection"))) {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Proxy-Connection", "Keep-Alive");
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(ver == PROXY_HTTP_V1) {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Connection", "Upgrade");
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Upgrade", "connect-udp");
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Capsule-Protocol", "?1");
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा {
    परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, ":Protocol", "connect-udp");
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    यदि(ver >= PROXY_HTTP_V2) {
      परिणाम = Curl_dynhds_cadd(&req->शीर्षकाणि, "Capsule-Protocol", "?1");
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }

  परिणाम = dynhds_सङ्कलय_custom(दत्त, TRUE, httpversion,
                             TRUE, &req->शीर्षकाणि);

बहिर्गमनम्:
  यदि(परिणाम && req) {
    Curl_http_req_free(req);
    req = रिक्त;
  }
  curlx_free(authority);
  curlx_free(मार्गः);
  curlx_free(encoded_host);
  *preq = req;
  प्रत्यागम परिणाम;
}

CURLcode Curl_http_proxy_create_tunnel_request(
    संरचना httpreq **preq, संरचना Curl_cfilter *cf,
    संरचना Curl_easy *दत्त, संरचना Curl_peer *peer, संरचना Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel)
{
  CURLcode परिणाम = udp_tunnel ?
    http_proxy_create_CONNECTUDP(preq, cf, दत्त, peer, dest, ver) :
    http_proxy_create_CONNECT(preq, cf, दत्त, peer, dest, ver);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  infof(दत्त, "Establishing %s proxy %stunnel to %s:%u",
        (ver == PROXY_HTTP_V2) ? "HTTP/2" :
        (ver == PROXY_HTTP_V3) ? "HTTP/3" : "HTTP",
        udp_tunnel ? "UDP " : "",
        dest->user_hostname, dest->पत्तनम्);
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_http_proxy_inspect_tunnel_response(
    संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
    संरचना http_resp *resp, bool udp_tunnel,
    proxy_inspect_परिणाम *presult)
{
  संरचना dynhds_प्रविष्टिः *capsule_protocol = रिक्त;
  संरचना dynhds_प्रविष्टिः *auth_प्रत्युत्तरम् = रिक्त;
  size_t i, शीर्षकम्_गणनम्;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(resp);

  शीर्षकम्_गणनम् = Curl_dynhds_गणनम्(&resp->शीर्षकाणि);
  infof(दत्त, "CONNECT%s Response Status %d",
        udp_tunnel ? "-UDP" : "", resp->स्थितिः);
  infof(दत्त, "Response Headers (%zu total):", शीर्षकम्_गणनम्);
  प्रति(i = 0; i < शीर्षकम्_गणनम्; i++) {
    संरचना dynhds_प्रविष्टिः *प्रविष्टिः = Curl_dynhds_getn(&resp->शीर्षकाणि, i);
    यदि(प्रविष्टिः)
      infof(दत्त, "  %s: %s", प्रविष्टिः->name, प्रविष्टिः->मानम्);
  }

  यदि(resp->स्थितिः == 407) {
    auth_प्रत्युत्तरम् = Curl_dynhds_cget(&resp->शीर्षकाणि, "Proxy-Authenticate");
  }

  यदि(auth_प्रत्युत्तरम्) {
    CURL_TRC_CF(दत्त, cf, "[0] CONNECT%s: fwd auth header '%s'",
                udp_tunnel ? "-UDP" : "", auth_प्रत्युत्तरम्->मानम्);
    परिणाम = Curl_http_निवेशः_auth(दत्त, resp->स्थितिः == 407,
                                  auth_प्रत्युत्तरम्->मानम्);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(दत्त->req.newurl) {
      curlx_safefree(दत्त->req.newurl);
      *presult = PROXY_INSPECT_AUTH_RETRY;
      प्रत्यागम CURLE_OK;
    }
  }

  यदि(udp_tunnel) {
    यदि(resp->स्थितिः / 100 == 2) {
      capsule_protocol = Curl_dynhds_cget(&resp->शीर्षकाणि,
                                           "capsule-protocol");
      यदि(capsule_protocol) {
        यदि(!वाक्ये_तुलय(capsule_protocol->मानम्, "?1", 2) &&
           !capsule_protocol->मानम्[2]) {
          infof(दत्त, "CONNECT-UDP tunnel established, response %d",
                resp->स्थितिः);
          *presult = PROXY_INSPECT_OK;
          प्रत्यागम CURLE_OK;
        }
        failf(दत्त, "Failed to establish CONNECT-UDP tunnel, response %d, "
              "unsupported capsule-protocol value '%s'",
              resp->स्थितिः, capsule_protocol->मानम्);
        *presult = PROXY_INSPECT_FAILED;
        प्रत्यागम CURLE_COULDNT_CONNECT;
      }
      अन्यथा {
        /* NOTE proxies may not set capsule protocol in the headers */
        infof(दत्त, "CONNECT-UDP tunnel established, response %d "
                    "but no capsule-protocol header found", resp->स्थितिः);
        *presult = PROXY_INSPECT_OK;
        प्रत्यागम CURLE_OK;
      }
    }
    अन्यथा {
      failf(दत्त, "Failed to establish CONNECT-UDP tunnel, "
                  "response %d", resp->स्थितिः);
      *presult = PROXY_INSPECT_FAILED;
      प्रत्यागम CURLE_COULDNT_CONNECT;
    }
  }

  यदि(resp->स्थितिः / 100 == 2) {
    infof(दत्त, "CONNECT tunnel established, response %d", resp->स्थितिः);
    *presult = PROXY_INSPECT_OK;
    प्रत्यागम CURLE_OK;
  }

  *presult = PROXY_INSPECT_FAILED;
  प्रत्यागम CURLE_COULDNT_CONNECT;
}

स्थिर CURLcode http_proxy_cf_connect(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      bool *समाप्तम्)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  bool udp_tunnel = TRNSPRT_IS_DGRAM(ctx->tunnel_transport);

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  CURL_TRC_CF(दत्त, cf, "CONNECT%s", udp_tunnel ? "-UDP" : "");
connect_sub:
  /* in case of h3_proxy, cf->next will be NULL initially */
  यदि(cf->अग्र) {
    परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  *समाप्तम् = FALSE;
  यदि(!ctx->sub_filter_installed) {
    ध्रुव वर्ण *alpn = रिक्त;

    /* in case of h3_proxy, cf->next will be NULL initially */
    यदि(cf->अग्र) {
      alpn = Curl_conn_cf_get_alpn_negotiated(cf->अग्र, दत्त);
    }

    यदि(alpn)
      infof(दत्त, "CONNECT%s: '%s' negotiated",
            udp_tunnel ? "-UDP" : "", alpn);
    अन्यथा यदि(!alpn) {
      /* No ALPN, proxytype rules. Fake ALPN */
      infof(दत्त, "CONNECT%s: no ALPN negotiated", udp_tunnel ? "-UDP" : "");
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

    यदि(!वाक्यं_तुलय(alpn, "http/1.0")) {
      CURL_TRC_CF(दत्त, cf, "installing subfilter for HTTP/1.0");
      परिणाम = Curl_cf_h1_proxy_निवेशय_after(cf, दत्त, ctx->peer,
                                             ctx->tunnel_peer, 10, udp_tunnel);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(!वाक्यं_तुलय(alpn, "http/1.1")) {
      पूर्णाङ्क httpversion = (ctx->proxytype == CURLPROXY_HTTP_1_0) ? 10 : 11;
      CURL_TRC_CF(दत्त, cf, "installing subfilter for HTTP/1.%d",
                  httpversion % 10);
      परिणाम = Curl_cf_h1_proxy_निवेशय_after(cf, दत्त, ctx->peer,
                                             ctx->tunnel_peer,
                                             httpversion, udp_tunnel);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
#यदि_परिभाषितम् USE_NGHTTP2
    अन्यथा यदि(!वाक्यं_तुलय(alpn, "h2")) {
      CURL_TRC_CF(दत्त, cf, "installing subfilter for HTTP/2");
      परिणाम = Curl_cf_h2_proxy_निवेशय_after(cf, दत्त, ctx->peer,
                                             ctx->tunnel_peer, udp_tunnel);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
#यदि_अन्तः /* USE_NGHTTP2 */
#यदि defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) && \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)
    अन्यथा यदि(!वाक्यं_तुलय(alpn, "h3")) {
      CURL_TRC_CF(दत्त, cf, "installing subfilter for HTTP/3");
      परिणाम = Curl_cf_h3_proxy_निवेशय_after(cf, दत्त, ctx->peer, ctx->peer,
                                             ctx->tunnel_peer,
                                             ctx->tunnel_transport);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
#यदि_अन्तः /* USE_PROXY_HTTP3 && USE_NGHTTP3 && USE_NGTCP2 && USE_OPENSSL */
    अन्यथा {
      failf(दत्त, "CONNECT%s: negotiated ALPN '%s' not supported",
            udp_tunnel ? "-UDP" : "", alpn);
      परिणाम = CURLE_COULDNT_CONNECT;
      गच्छ बहिर्गमनम्;
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
    परिणाम = CURLE_OK;
  }

बहिर्गमनम्:
  यदि(!परिणाम) {
    cf->connected = TRUE;
    *समाप्तम् = TRUE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_http_proxy_query(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  विकल्प(query) {
  पक्ष CF_QUERY_HOST_पत्तनम्:
    *pres1 = (पूर्णाङ्क)ctx->tunnel_peer->पत्तनम्;
    *((ध्रुव वर्ण **)pres2) = ctx->tunnel_peer->hostname;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
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
                                  संरचना Curl_easy *दत्त)
{
  संरचना cf_proxy_ctx *ctx = cf->ctx;
  यदि(ctx) {
    CURL_TRC_CF(दत्त, cf, "destroy");
    cf_https_proxy_ctx_free(ctx);
  }
}

संरचना Curl_cftype Curl_cft_http_proxy = {
  "HTTP-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_सज्जता,
  0,
  http_proxy_cf_destroy,
  http_proxy_cf_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_http_proxy_query,
};

CURLcode Curl_cf_http_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                         संरचना Curl_easy *दत्त,
                                         संरचना Curl_peer *peer,
                                         संरचना Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_proxy_ctx *ctx = रिक्त;
  CURLcode परिणाम;

  (शून्य)दत्त;
  यदि(!peer || !tunnel_peer)
    प्रत्यागम CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->tunnel_peer, tunnel_peer);
  ctx->proxytype = proxytype;
  ctx->tunnel_transport = tunnel_transport;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_http_proxy, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ctx = रिक्त;
  Curl_conn_cf_निवेशय_after(cf_at, cf);

बहिर्गमनम्:
  cf_https_proxy_ctx_free(ctx);
  प्रत्यागम परिणाम;
}

uint8_t Curl_http_proxy_transport(uint8_t proxytype)
{
  विकल्प(proxytype) {
  पक्ष CURLPROXY_HTTPS3:
    प्रत्यागम TRNSPRT_QUIC;
  स्वभाव:
    प्रत्यागम TRNSPRT_TCP;
  }
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY */
