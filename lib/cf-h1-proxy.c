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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#ਅਨ੍ਤਰ੍ਭਾਵ <curl/curl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-h1-proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
ਬਣਤਰ h1_tunnel_state {
  ਬਣਤਰ Curl_peer *dest;
  ਬਣਤਰ dynbuf rcvbuf;
  ਬਣਤਰ dynbuf request_ਅੰਕੜੇ;
  size_t nsent;
  size_t headerlines;
  ਬਣਤਰ Curl_chunker ch;
  ਪੂਰਨ httpversion;
  ਗਿਣਤੀ keeponval {
    KEEPON_ਪੂਰਾ,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  ਦੋਆਧਾਰੀ_ਅੰਕ(chunked_encoding);
  ਦੋਆਧਾਰੀ_ਅੰਕ(close_connection);
  ਦੋਆਧਾਰੀ_ਅੰਕ(maybe_folded);
  ਦੋਆਧਾਰੀ_ਅੰਕ(leading_unfold);
  ਦੋਆਧਾਰੀ_ਅੰਕ(udp_tunnel);
};

/* Persistent context for the H1-PROXY filter */
ਬਣਤਰ cf_h1_proxy_ctx {
  ਬਣਤਰ Curl_peer *peer;
  ਬਣਤਰ h1_tunnel_state *ts;
};

ਸਥਿਰ bool tunnel_is_established(ਬਣਤਰ h1_tunnel_state *ts)
{
  ਵਾਪਸ ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

ਸਥਿਰ bool tunnel_is_failed(ਬਣਤਰ h1_tunnel_state *ts)
{
  ਵਾਪਸ ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

ਸਥਿਰ bool h1_proxy_is_udp(ਬਣਤਰ Curl_cfilter *cf)
{
  ਬਣਤਰ cf_h1_proxy_ctx *pctx = cf->ctx;
  ਵਾਪਸ (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

ਸਥਿਰ CURLcode tunnel_reinit(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ h1_tunnel_state *ts)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_ਅੰਕੜੇ);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode tunnel_init(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ h1_tunnel_state **pts)
{
  ਬਣਤਰ h1_tunnel_state *ts;

  ਜੇ(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(ਅੰਕੜੇ, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, ਆਕਾਰ(*ts));
  ਜੇ(!ts)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  infof(ਅੰਕੜੇ, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_ਸਿਰਲੇਖਾਂ);
  curlx_dyn_init(&ts->request_ਅੰਕੜੇ, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(ਅੰਕੜੇ, &ts->ch, TRUE, TRUE);

  *pts = ts;
  ਵਾਪਸ tunnel_reinit(cf, ਅੰਕੜੇ, ts);
}

ਸਥਿਰ ਖਾਲੀ h1_tunnel_go_state(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ts->tunnel_state == new_state)
    ਵਾਪਸ;
  /* entering this one */
  ਚੋਣ(new_state) {
  ਹਾਲਤ H1_TUNNEL_INIT:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, ਅੰਕੜੇ, ts);
    ਰੋਕੋ;

  ਹਾਲਤ H1_TUNNEL_CONNECT:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    ਰੋਕੋ;

  ਹਾਲਤ H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    ਰੋਕੋ;

  ਹਾਲਤ H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    ਰੋਕੋ;

  ਹਾਲਤ H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "new tunnel state 'established'");
    infof(ਅੰਕੜੇ, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    ਅੰਕੜੇ->state.authproxy.ਪੂਰਾ = TRUE;
    ਅੰਕੜੇ->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ਹਾਲਤ H1_TUNNEL_FAILED:
    ਜੇ(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_ਅੰਕੜੇ);
    /* restore the protocol pointer */
    ਅੰਕੜੇ->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(ਅੰਕੜੇ->req.hd_proxy_auth);
    ਰੋਕੋ;
  }
}

ਸਥਿਰ ਖਾਲੀ tunnel_free(ਬਣਤਰ h1_tunnel_state *ts,
                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_ਅੰਕੜੇ);
    Curl_httpchunk_free(ਅੰਕੜੇ, &ts->ch);
    curlx_free(ts);
  }
}

ਸਥਿਰ ਖਾਲੀ cf_tunnel_free(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(cf) {
    ਬਣਤਰ cf_h1_proxy_ctx *pctx = cf->ctx;
    ਬਣਤਰ h1_tunnel_state *ts = pctx ? pctx->ts : ਨਿਰਾ;
    ਜੇ(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ਅੰਕੜੇ);
      tunnel_free(ts, ਅੰਕੜੇ);
      pctx->ts = ਨਿਰਾ;
    }
  }
}

ਸਥਿਰ bool tunnel_want_send(ਬਣਤਰ h1_tunnel_state *ts)
{
  ਵਾਪਸ ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ਸਥਿਰ CURLcode ਸ਼ੁਰੂ_CONNECT(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ h1_tunnel_state *ts)
{
  ਬਣਤਰ cf_h1_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ httpreq *req = ਨਿਰਾ;
  ਪੂਰਨ http_minor;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(ਅੰਕੜੇ->req.newurl);

  ਨਤੀਜਾ = Curl_http_proxy_create_tunnel_request(&req, cf, ਅੰਕੜੇ,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  curlx_dyn_reset(&ts->request_ਅੰਕੜੇ);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  ਨਤੀਜਾ = Curl_h1_req_write_ਸਿਰ(req, http_minor, &ts->request_ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "Failed sending CONNECT to proxy");
  ਜੇ(req)
    Curl_http_req_free(req);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode send_CONNECT(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ h1_tunnel_state *ts,
                             bool *ਪੂਰਾ)
{
  ਅਚਲ uint8_t *buf = curlx_dyn_uptr(&ts->request_ਅੰਕੜੇ);
  size_t request_len = curlx_dyn_len(&ts->request_ਅੰਕੜੇ);
  size_t blen = request_len;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nwritten;

  ਜੇ(blen <= ts->nsent)
    ਜਾਓ ਬਾਹਰਲਾ;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, blen, FALSE, &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਨਤੀਜਾ = CURLE_OK;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, (ਅਚਲ ਅੱਖਰ *)buf, nwritten);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "Failed sending CONNECT to proxy");
  *ਪੂਰਾ = (!ਨਤੀਜਾ && (ts->nsent >= request_len));
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode on_resp_ਸਥਿਤੀ(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ h1_tunnel_state *ts,
                               ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;

  (ਖਾਲੀ)ts;
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(ਸਿਰਲੇਖ, "HTTP/1.", 7) &&
          ((ਸਿਰਲੇਖ[7] == '0') || (ਸਿਰਲੇਖ[7] == '1')) &&
          (ਸਿਰਲੇਖ[8] == ' ') &&
          ISDIGIT(ਸਿਰਲੇਖ[9]) && ISDIGIT(ਸਿਰਲੇਖ[10]) && ISDIGIT(ਸਿਰਲੇਖ[11]) &&
          !ISDIGIT(ਸਿਰਲੇਖ[12])) {
    /* store the HTTP code from the proxy */
    ਅੰਕੜੇ->info.httpproxycode = k->httpcode = ((ਸਿਰਲੇਖ[9] - '0') * 100) +
      ((ਸਿਰਲੇਖ[10] - '0') * 10) + (ਸਿਰਲੇਖ[11] - '0');
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    ਵਾਪਸ CURLE_OK;
  }
  failf(ਅੰਕੜੇ, "Invalid response header");
  ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
}

ਸਥਿਰ CURLcode on_resp_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ h1_tunnel_state *ts,
                               ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  bool is_udp = h1_proxy_is_udp(cf);

  ਜੇ(checkprefix("Proxy-authenticate:", ਸਿਰਲੇਖ) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    ਅੱਖਰ *auth = Curl_copy_ਸਿਰਲੇਖ_ਮੁੱਲ(ਸਿਰਲੇਖ);
    ਜੇ(!auth)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", ਸਿਰਲੇਖ);
    ਨਤੀਜਾ = Curl_http_ਨਿਵੇਸ਼_auth(ਅੰਕੜੇ, proxy, auth);

    curlx_free(auth);

    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("Content-Length:", ਸਿਰਲੇਖ)) {
    ਜੇ(k->httpcode < 300) {
      ਜੇ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(ਅੰਕੜੇ, "Invalid Content-Length: in %03d response", k->httpcode);
        ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(ਅੰਕੜੇ, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    ਨਹੀਂਤਾਂ {
      ਅਚਲ ਅੱਖਰ *p = ਸਿਰਲੇਖ + CURL_CSTRLEN("Content-Length:");
      ਜੇ(curlx_str_numblanks(&p, &ts->cl)) {
        failf(ਅੰਕੜੇ, "Unsupported Content-Length value");
        ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
      }
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(Curl_compareheader(ਸਿਰਲੇਖ,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("Transfer-Encoding:", ਸਿਰਲੇਖ)) {
    ਜੇ(k->httpcode < 300) {
      ਜੇ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(ਅੰਕੜੇ, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(ਅੰਕੜੇ, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    ਨਹੀਂਤਾਂ ਜੇ(Curl_compareheader(ਸਿਰਲੇਖ,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(ਅੰਕੜੇ, &ts->ch, TRUE);
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(is_udp && checkprefix("Capsule-protocol:", ਸਿਰਲੇਖ)) {
    ਜੇ(Curl_compareheader(ਸਿਰਲੇਖ,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(Curl_compareheader(ਸਿਰਲੇਖ,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode single_ਸਿਰਲੇਖ(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ h1_tunnel_state *ts)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t ਸਤਰ_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ਅਚਲ ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  ਪੂਰਨ writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, linep, ਸਤਰ_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  ਨਤੀਜਾ = Curl_verify_ਸਿਰਲੇਖ(ਅੰਕੜੇ, linep, ਸਤਰ_len);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* send the header to the callback */
  writetype = CLIENTWRITE_ਸਿਰਲੇਖ | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_ਸਥਿਤੀ : 0);
  ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, writetype, linep, ਸਤਰ_len);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_bump_headersize(ਅੰਕੜੇ, ਸਤਰ_len, TRUE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  ਜੇ(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    ਜੇ(ts->headerlines == 1) {
      ts->keepon = KEEPON_ਪੂਰਾ;
      ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
    }
    ਨਹੀਂਤਾਂ ਜੇ((407 == k->httpcode) && !ਅੰਕੜੇ->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      ਜੇ(ts->cl) {
        infof(ਅੰਕੜੇ, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      ਨਹੀਂਤਾਂ ਜੇ(ts->chunked_encoding) {
        infof(ਅੰਕੜੇ, "Ignore chunked response-body");
      }
      ਨਹੀਂਤਾਂ {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_ਪੂਰਾ;
      }
    }
    ਨਹੀਂਤਾਂ {
      ts->keepon = KEEPON_ਪੂਰਾ;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_ਪੂਰਾ);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(ts->headerlines == 1)
    ਨਤੀਜਾ = on_resp_ਸਥਿਤੀ(cf, ਅੰਕੜੇ, ts, linep);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = on_resp_ਸਿਰਲੇਖ(cf, ਅੰਕੜੇ, ts, linep);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  curlx_dyn_reset(&ts->rcvbuf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode recv_CONNECT_resp(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ h1_tunnel_state *ts,
                                  bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ error;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SELECT_OK      0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SELECT_ERROR   1

  error = SELECT_OK;
  *ਪੂਰਾ = FALSE;

  ਜਦੋਂ(ts->keepon) {
    size_t nread;
    ਅੱਖਰ byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    ਨਤੀਜਾ = Curl_conn_recv(ਅੰਕੜੇ, cf->sockindex, &byte, 1, &nread);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      /* socket buffer drained, return */
      ਵਾਪਸ CURLE_OK;

    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);

    ਜੇ(ਨਤੀਜਾ) {
      ts->keepon = KEEPON_ਪੂਰਾ;
      ਰੋਕੋ;
    }

    ਜੇ(!nread) {
      ਜੇ(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        ਨਤੀਜਾ = single_ਸਿਰਲੇਖ(cf, ਅੰਕੜੇ, ts);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
        ts->maybe_folded = FALSE;
      }
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.proxyauth && ਅੰਕੜੇ->state.authproxy.avail &&
         ਅੰਕੜੇ->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(ਅੰਕੜੇ, "Proxy CONNECT connection closed");
      }
      ਨਹੀਂਤਾਂ {
        error = SELECT_ERROR;
        failf(ਅੰਕੜੇ, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_ਪੂਰਾ;
      ਰੋਕੋ;
    }

    ਜੇ(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      ਜੇ(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        ਨਤੀਜਾ = Curl_httpchunk_read(ਅੰਕੜੇ, &ts->ch, &byte, 1, &consumed);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
        ਜੇ(Curl_httpchunk_is_ਪੂਰਾ(ਅੰਕੜੇ, &ts->ch)) {
          /* we are done reading chunks! */
          infof(ਅੰਕੜੇ, "chunk reading DONE");
          ts->keepon = KEEPON_ਪੂਰਾ;
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        ਜੇ(ts->cl <= 0) {
          ts->keepon = KEEPON_ਪੂਰਾ;
          ਰੋਕੋ;
        }
      }
      ਜਾਰੀ;
    }

    ਜੇ(ts->maybe_folded) {
      ਜੇ(ISBLANK(byte)) {
        Curl_http_ਮੰਜ਼ਿਲ_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = single_ਸਿਰਲੇਖ(cf, ਅੰਕੜੇ, ts);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    ਜੇ(ts->leading_unfold) {
      ਜੇ(ISBLANK(byte))
        /* skip a bit brother */
        ਜਾਰੀ;
      /* non-blank, insert a space then continue the unfolding */
      ਜੇ(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(ਅੰਕੜੇ, "CONNECT response too large");
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    ਜੇ(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(ਅੰਕੜੇ, "CONNECT response too large");
      ਵਾਪਸ CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    ਜੇ(byte != 0x0a)
      ਜਾਰੀ;
    ਨਹੀਂਤਾਂ {
      ਅਚਲ ਅੱਖਰ *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      ਜੇ(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        ਨਤੀਜਾ = single_ਸਿਰਲੇਖ(cf, ਅੰਕੜੇ, ts);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਹੀਂਤਾਂ
        ts->maybe_folded = TRUE;
    }

    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  } /* while there is buffer left and loop is requested */

  ਜੇ(error)
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
  *ਪੂਰਾ = (ts->keepon == KEEPON_ਪੂਰਾ);
  ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ &&
     ਅੰਕੜੇ->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && ਅੰਕੜੇ->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    ਨਤੀਜਾ = Curl_http_auth_act(ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode H1_CONNECT(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ h1_tunnel_state *ts)
{
  ਬਣਤਰ connectdata *conn = cf->conn;
  CURLcode ਨਤੀਜਾ;
  bool ਪੂਰਾ;

  ਜੇ(tunnel_is_established(ts))
    ਵਾਪਸ CURLE_OK;
  ਜੇ(tunnel_is_failed(ts))
    ਵਾਪਸ CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  ਕਰੋ {

    ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
      failf(ਅੰਕੜੇ, "Proxy CONNECT aborted due to timeout");
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਚੋਣ(ts->tunnel_state) {
    ਹਾਲਤ H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT start");
      ਨਤੀਜਾ = ਸ਼ੁਰੂ_CONNECT(cf, ਅੰਕੜੇ, ts);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, ਅੰਕੜੇ);
      FALLTHROUGH();

    ਹਾਲਤ H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT send");
      ਨਤੀਜਾ = send_CONNECT(cf, ਅੰਕੜੇ, ts, &ਪੂਰਾ);
      ਜੇ(ਨਤੀਜਾ || !ਪੂਰਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, ਅੰਕੜੇ);
      FALLTHROUGH();

    ਹਾਲਤ H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT receive");
      ਨਤੀਜਾ = recv_CONNECT_resp(cf, ਅੰਕੜੇ, ts, &ਪੂਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "error receiving CONNECT response: %d",
                    (ਪੂਰਨ)ਨਤੀਜਾ);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);
      /* error or not complete yet. return for more multi-multi */
      ਜੇ(ਨਤੀਜਾ || !ਪੂਰਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, ਅੰਕੜੇ);
      FALLTHROUGH();

    ਹਾਲਤ H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT response");
      ਜੇ(ਅੰਕੜੇ->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
        ਜੇ(ts->close_connection || conn->ਬਿੱਟ.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(ਅੰਕੜੇ, cf, "CONNECT need to close+open");
          infof(ਅੰਕੜੇ, "Connect me again please");
          ਵਾਪਸ CURLE_AGAIN;
        }
        ਨਹੀਂਤਾਂ {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, ਅੰਕੜੇ);
        }
      }
      ਰੋਕੋ;

    ਮੂਲ:
      ਰੋਕੋ;
    }

  } ਜਦੋਂ(ਅੰਕੜੇ->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  ਜੇ(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    ਜੇ(ਅੰਕੜੇ->info.httpproxycode / 100 != 2 &&
       ਅੰਕੜੇ->info.httpproxycode != 101) {
      curlx_safefree(ਅੰਕੜੇ->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ਅੰਕੜੇ);
      failf(ਅੰਕੜੇ, "CONNECT-UDP tunnel failed, response %d",
            ਅੰਕੜੇ->req.httpcode);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(ਅੰਕੜੇ->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(ਅੰਕੜੇ->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ਅੰਕੜੇ);
      failf(ਅੰਕੜੇ, "CONNECT tunnel failed, response %d", ਅੰਕੜੇ->req.httpcode);
      ਵਾਪਸ CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, ਅੰਕੜੇ);
  infof(ਅੰਕੜੇ, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", ਅੰਕੜੇ->info.httpproxycode);
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h1_proxy_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ cf_h1_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ h1_tunnel_state *ts = pctx->ts;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "connect");
  ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  *ਪੂਰਾ = FALSE;
  ਜੇ(!ts) {
    ਨਤੀਜਾ = tunnel_init(cf, ਅੰਕੜੇ, &ts);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  ਨਤੀਜਾ = H1_CONNECT(cf, ਅੰਕੜੇ, ts);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  curlx_safefree(ਅੰਕੜੇ->req.hd_proxy_auth);

ਬਾਹਰਲਾ:
  *ਪੂਰਾ = (ਨਤੀਜਾ == CURLE_OK) && tunnel_is_established(pctx->ts);
  ਜੇ(*ਪੂਰਾ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
    Curl_client_reset(ਅੰਕੜੇ);
    Curl_pgrsReset(ਅੰਕੜੇ);
    cf_tunnel_free(cf, ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h1_proxy_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_h1_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ h1_tunnel_state *ts = pctx->ts;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);
    ਜੇ(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      ਜੇ(tunnel_want_send(ts))
        ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(ਅੰਕੜੇ, ps, sock);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਵਿੱਚ_only(ਅੰਕੜੇ, ps, sock);
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(ਅੰਕੜੇ, ps, sock);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(cf->ਅਗਲਾ)
      ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->adjust_pollset(cf->ਅਗਲਾ, ਅੰਕੜੇ, ps);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_h1_proxy_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                     ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ cf->ਅਗਲਾ ? cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ) : FALSE;
}

ਸਥਿਰ ਖਾਲੀ cf_h1_proxy_destroy(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  ਜੇ(pctx) {
    cf_tunnel_free(cf, ਅੰਕੜੇ);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ਸਥਿਰ CURLcode cf_h1_proxy_query(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_h1_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ h1_tunnel_state *ts = pctx ? pctx->ts : ਨਿਰਾ;
  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_HOST_ਪੋਰਟ:
    ਜੇ(!ts || !ts->dest)
      ਰੋਕੋ;
    *pres1 = (ਪੂਰਨ)ts->dest->ਪੋਰਟ;
    *((ਅਚਲ ਅੱਖਰ **)pres2) = ts->dest->hostname;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_ALPN_NEGOTIATED: {
    ਅਚਲ ਅੱਖਰ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ਨਿਰਾ;
    ਵਾਪਸ CURLE_OK;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਬਣਤਰ Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_peer *peer,
                                       ਬਣਤਰ Curl_peer *dest,
                                       ਪੂਰਨ httpversion,
                                       bool udp_tunnel)
{
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ cf_h1_proxy_ctx *pctx;
  ਬਣਤਰ h1_tunnel_state *ts;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(!dest)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ((httpversion < 10) || (httpversion >= 20))
    ਵਾਪਸ CURLE_FAILED_INIT;

  ts = curlx_calloc(1, ਆਕਾਰ(*ts));
  ਜੇ(!ts) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_ਸਿਰਲੇਖਾਂ);
  curlx_dyn_init(&ts->request_ਅੰਕੜੇ, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(ਅੰਕੜੇ, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, ਆਕਾਰ(*pctx));
  ਜੇ(!pctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  ਜੇ(ਨਤੀਜਾ) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ts = ਨਿਰਾ;
  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);

ਬਾਹਰਲਾ:
  tunnel_free(ts, ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
