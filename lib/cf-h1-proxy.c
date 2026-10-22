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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#ಅನ್ತರ್ಭಾವ <curl/curl.h>
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-h1-proxy.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
ರಚನೆ h1_tunnel_state {
  ರಚನೆ Curl_peer *dest;
  ರಚನೆ dynbuf rcvbuf;
  ರಚನೆ dynbuf request_ದತ್ತಾಂಶ;
  size_t nsent;
  size_t headerlines;
  ರಚನೆ Curl_chunker ch;
  ಪೂರ್ಣಾಂಕ httpversion;
  ಎಣಿಕೆ keeponval {
    KEEPON_ಮುಗಿದ,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  ಬಿಟ್(chunked_encoding);
  ಬಿಟ್(close_connection);
  ಬಿಟ್(maybe_folded);
  ಬಿಟ್(leading_unfold);
  ಬಿಟ್(udp_tunnel);
};

/* Persistent context for the H1-PROXY filter */
ರಚನೆ cf_h1_proxy_ctx {
  ರಚನೆ Curl_peer *peer;
  ರಚನೆ h1_tunnel_state *ts;
};

ಸ್ಥಿರ bool tunnel_is_established(ರಚನೆ h1_tunnel_state *ts)
{
  ಹಿಂತಿರುಗಿ ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

ಸ್ಥಿರ bool tunnel_is_failed(ರಚನೆ h1_tunnel_state *ts)
{
  ಹಿಂತಿರುಗಿ ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

ಸ್ಥಿರ bool h1_proxy_is_udp(ರಚನೆ Curl_cfilter *cf)
{
  ರಚನೆ cf_h1_proxy_ctx *pctx = cf->ctx;
  ಹಿಂತಿರುಗಿ (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

ಸ್ಥಿರ CURLcode tunnel_reinit(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ h1_tunnel_state *ts)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_ದತ್ತಾಂಶ);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tunnel_init(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ h1_tunnel_state **pts)
{
  ರಚನೆ h1_tunnel_state *ts;

  ಆದರೆ(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(ದತ್ತಾಂಶ, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, ಗಾತ್ರ(*ts));
  ಆದರೆ(!ts)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  infof(ದತ್ತಾಂಶ, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_ಶೀರ್ಷಿಕೆಗಳು);
  curlx_dyn_init(&ts->request_ದತ್ತಾಂಶ, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(ದತ್ತಾಂಶ, &ts->ch, TRUE, TRUE);

  *pts = ts;
  ಹಿಂತಿರುಗಿ tunnel_reinit(cf, ದತ್ತಾಂಶ, ts);
}

ಸ್ಥಿರ ಶೂನ್ಯ h1_tunnel_go_state(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ts->tunnel_state == new_state)
    ಹಿಂತಿರುಗಿ;
  /* entering this one */
  ಆಯ್ಕೆ(new_state) {
  ಸ್ಥಿತಿ H1_TUNNEL_INIT:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, ದತ್ತಾಂಶ, ts);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_TUNNEL_CONNECT:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "new tunnel state 'established'");
    infof(ದತ್ತಾಂಶ, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    ದತ್ತಾಂಶ->state.authproxy.ಮುಗಿದ = TRUE;
    ದತ್ತಾಂಶ->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ಸ್ಥಿತಿ H1_TUNNEL_FAILED:
    ಆದರೆ(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_ದತ್ತಾಂಶ);
    /* restore the protocol pointer */
    ದತ್ತಾಂಶ->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(ದತ್ತಾಂಶ->req.hd_proxy_auth);
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ tunnel_free(ರಚನೆ h1_tunnel_state *ts,
                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_ದತ್ತಾಂಶ);
    Curl_httpchunk_free(ದತ್ತಾಂಶ, &ts->ch);
    curlx_free(ts);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_tunnel_free(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(cf) {
    ರಚನೆ cf_h1_proxy_ctx *pctx = cf->ctx;
    ರಚನೆ h1_tunnel_state *ts = pctx ? pctx->ts : ಖಾಲಿ;
    ಆದರೆ(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ದತ್ತಾಂಶ);
      tunnel_free(ts, ದತ್ತಾಂಶ);
      pctx->ts = ಖಾಲಿ;
    }
  }
}

ಸ್ಥಿರ bool tunnel_want_send(ರಚನೆ h1_tunnel_state *ts)
{
  ಹಿಂತಿರುಗಿ ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ಸ್ಥಿರ CURLcode ಆರಂಭ_CONNECT(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ h1_tunnel_state *ts)
{
  ರಚನೆ cf_h1_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ httpreq *req = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ http_minor;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(ದತ್ತಾಂಶ->req.newurl);

  ಫಲಿತಾಂಶ = Curl_http_proxy_create_tunnel_request(&req, cf, ದತ್ತಾಂಶ,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  curlx_dyn_reset(&ts->request_ದತ್ತಾಂಶ);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  ಫಲಿತಾಂಶ = Curl_h1_req_write_ತಲೆ(req, http_minor, &ts->request_ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "Failed sending CONNECT to proxy");
  ಆದರೆ(req)
    Curl_http_req_free(req);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode send_CONNECT(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ h1_tunnel_state *ts,
                             bool *ಮುಗಿದ)
{
  ಸ್ಥಿರಾಂಕ uint8_t *buf = curlx_dyn_uptr(&ts->request_ದತ್ತಾಂಶ);
  size_t request_len = curlx_dyn_len(&ts->request_ದತ್ತಾಂಶ);
  size_t blen = request_len;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nwritten;

  ಆದರೆ(blen <= ts->nsent)
    ಹೋಗು ಹೊರಹೋಗು;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, blen, FALSE, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಫಲಿತಾಂಶ = CURLE_OK;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, nwritten);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "Failed sending CONNECT to proxy");
  *ಮುಗಿದ = (!ಫಲಿತಾಂಶ && (ts->nsent >= request_len));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode on_resp_ಸ್ಥಿತಿ(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ h1_tunnel_state *ts,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;

  (ಶೂನ್ಯ)ts;
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಶೀರ್ಷಿಕೆ, "HTTP/1.", 7) &&
          ((ಶೀರ್ಷಿಕೆ[7] == '0') || (ಶೀರ್ಷಿಕೆ[7] == '1')) &&
          (ಶೀರ್ಷಿಕೆ[8] == ' ') &&
          ISDIGIT(ಶೀರ್ಷಿಕೆ[9]) && ISDIGIT(ಶೀರ್ಷಿಕೆ[10]) && ISDIGIT(ಶೀರ್ಷಿಕೆ[11]) &&
          !ISDIGIT(ಶೀರ್ಷಿಕೆ[12])) {
    /* store the HTTP code from the proxy */
    ದತ್ತಾಂಶ->info.httpproxycode = k->httpcode = ((ಶೀರ್ಷಿಕೆ[9] - '0') * 100) +
      ((ಶೀರ್ಷಿಕೆ[10] - '0') * 10) + (ಶೀರ್ಷಿಕೆ[11] - '0');
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  failf(ದತ್ತಾಂಶ, "Invalid response header");
  ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
}

ಸ್ಥಿರ CURLcode on_resp_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ h1_tunnel_state *ts,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  bool is_udp = h1_proxy_is_udp(cf);

  ಆದರೆ(checkprefix("Proxy-authenticate:", ಶೀರ್ಷಿಕೆ) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    ಅಕ್ಷರ *auth = Curl_copy_ಶೀರ್ಷಿಕೆ_ಮೌಲ್ಯ(ಶೀರ್ಷಿಕೆ);
    ಆದರೆ(!auth)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", ಶೀರ್ಷಿಕೆ);
    ಫಲಿತಾಂಶ = Curl_http_ಒಳಹರಿವು_auth(ದತ್ತಾಂಶ, proxy, auth);

    curlx_free(auth);

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("Content-Length:", ಶೀರ್ಷಿಕೆ)) {
    ಆದರೆ(k->httpcode < 300) {
      ಆದರೆ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(ದತ್ತಾಂಶ, "Invalid Content-Length: in %03d response", k->httpcode);
        ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(ದತ್ತಾಂಶ, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಶೀರ್ಷಿಕೆ + CURL_CSTRLEN("Content-Length:");
      ಆದರೆ(curlx_str_numblanks(&p, &ts->cl)) {
        failf(ದತ್ತಾಂಶ, "Unsupported Content-Length value");
        ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_compareheader(ಶೀರ್ಷಿಕೆ,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("Transfer-Encoding:", ಶೀರ್ಷಿಕೆ)) {
    ಆದರೆ(k->httpcode < 300) {
      ಆದರೆ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(ದತ್ತಾಂಶ, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(ದತ್ತಾಂಶ, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_compareheader(ಶೀರ್ಷಿಕೆ,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(ದತ್ತಾಂಶ, &ts->ch, TRUE);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(is_udp && checkprefix("Capsule-protocol:", ಶೀರ್ಷಿಕೆ)) {
    ಆದರೆ(Curl_compareheader(ಶೀರ್ಷಿಕೆ,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_compareheader(ಶೀರ್ಷಿಕೆ,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode single_ಶೀರ್ಷಿಕೆ(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ h1_tunnel_state *ts)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t ಸಾಲು_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ಸ್ಥಿರಾಂಕ ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  ಪೂರ್ಣಾಂಕ writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, linep, ಸಾಲು_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  ಫಲಿತಾಂಶ = Curl_verify_ಶೀರ್ಷಿಕೆ(ದತ್ತಾಂಶ, linep, ಸಾಲು_len);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* send the header to the callback */
  writetype = CLIENTWRITE_ಶೀರ್ಷಿಕೆ | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_ಸ್ಥಿತಿ : 0);
  ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, writetype, linep, ಸಾಲು_len);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_bump_headersize(ದತ್ತಾಂಶ, ಸಾಲು_len, TRUE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  ಆದರೆ(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    ಆದರೆ(ts->headerlines == 1) {
      ts->keepon = KEEPON_ಮುಗಿದ;
      ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((407 == k->httpcode) && !ದತ್ತಾಂಶ->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      ಆದರೆ(ts->cl) {
        infof(ದತ್ತಾಂಶ, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ts->chunked_encoding) {
        infof(ದತ್ತಾಂಶ, "Ignore chunked response-body");
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_ಮುಗಿದ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ts->keepon = KEEPON_ಮುಗಿದ;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(ts->headerlines == 1)
    ಫಲಿತಾಂಶ = on_resp_ಸ್ಥಿತಿ(cf, ದತ್ತಾಂಶ, ts, linep);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = on_resp_ಶೀರ್ಷಿಕೆ(cf, ದತ್ತಾಂಶ, ts, linep);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  curlx_dyn_reset(&ts->rcvbuf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode recv_CONNECT_resp(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ h1_tunnel_state *ts,
                                  bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ error;

#ವ್ಯಾಖ್ಯಾನಿಸು SELECT_OK      0
#ವ್ಯಾಖ್ಯಾನಿಸು SELECT_ERROR   1

  error = SELECT_OK;
  *ಮುಗಿದ = FALSE;

  ಆಗ(ts->keepon) {
    size_t nread;
    ಅಕ್ಷರ byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    ಫಲಿತಾಂಶ = Curl_conn_recv(ದತ್ತಾಂಶ, cf->sockindex, &byte, 1, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      /* socket buffer drained, return */
      ಹಿಂತಿರುಗಿ CURLE_OK;

    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);

    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ts->keepon = KEEPON_ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(!nread) {
      ಆದರೆ(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        ಫಲಿತಾಂಶ = single_ಶೀರ್ಷಿಕೆ(cf, ದತ್ತಾಂಶ, ts);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        ts->maybe_folded = FALSE;
      }
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.proxyauth && ದತ್ತಾಂಶ->state.authproxy.avail &&
         ದತ್ತಾಂಶ->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(ದತ್ತಾಂಶ, "Proxy CONNECT connection closed");
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        error = SELECT_ERROR;
        failf(ದತ್ತಾಂಶ, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_ಮುಗಿದ;
      ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      ಆದರೆ(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        ಫಲಿತಾಂಶ = Curl_httpchunk_read(ದತ್ತಾಂಶ, &ts->ch, &byte, 1, &consumed);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        ಆದರೆ(Curl_httpchunk_is_ಮುಗಿದ(ದತ್ತಾಂಶ, &ts->ch)) {
          /* we are done reading chunks! */
          infof(ದತ್ತಾಂಶ, "chunk reading DONE");
          ts->keepon = KEEPON_ಮುಗಿದ;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        ಆದರೆ(ts->cl <= 0) {
          ts->keepon = KEEPON_ಮುಗಿದ;
          ನಿಲ್ಲಿಸು;
        }
      }
      ಮುಂದುವರಿ;
    }

    ಆದರೆ(ts->maybe_folded) {
      ಆದರೆ(ISBLANK(byte)) {
        Curl_http_ಗಮ್ಯ_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = single_ಶೀರ್ಷಿಕೆ(cf, ದತ್ತಾಂಶ, ts);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    ಆದರೆ(ts->leading_unfold) {
      ಆದರೆ(ISBLANK(byte))
        /* skip a bit brother */
        ಮುಂದುವರಿ;
      /* non-blank, insert a space then continue the unfolding */
      ಆದರೆ(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(ದತ್ತಾಂಶ, "CONNECT response too large");
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    ಆದರೆ(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(ದತ್ತಾಂಶ, "CONNECT response too large");
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    ಆದರೆ(byte != 0x0a)
      ಮುಂದುವರಿ;
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      ಆದರೆ(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        ಫಲಿತಾಂಶ = single_ಶೀರ್ಷಿಕೆ(cf, ದತ್ತಾಂಶ, ts);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ts->maybe_folded = TRUE;
    }

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  } /* while there is buffer left and loop is requested */

  ಆದರೆ(error)
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
  *ಮುಗಿದ = (ts->keepon == KEEPON_ಮುಗಿದ);
  ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ &&
     ದತ್ತಾಂಶ->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && ದತ್ತಾಂಶ->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    ಫಲಿತಾಂಶ = Curl_http_auth_act(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode H1_CONNECT(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ h1_tunnel_state *ts)
{
  ರಚನೆ connectdata *conn = cf->conn;
  CURLcode ಫಲಿತಾಂಶ;
  bool ಮುಗಿದ;

  ಆದರೆ(tunnel_is_established(ts))
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಆದರೆ(tunnel_is_failed(ts))
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  ಮಾಡು {

    ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
      failf(ದತ್ತಾಂಶ, "Proxy CONNECT aborted due to timeout");
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆಯ್ಕೆ(ts->tunnel_state) {
    ಸ್ಥಿತಿ H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT start");
      ಫಲಿತಾಂಶ = ಆರಂಭ_CONNECT(cf, ದತ್ತಾಂಶ, ts);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, ದತ್ತಾಂಶ);
      FALLTHROUGH();

    ಸ್ಥಿತಿ H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT send");
      ಫಲಿತಾಂಶ = send_CONNECT(cf, ದತ್ತಾಂಶ, ts, &ಮುಗಿದ);
      ಆದರೆ(ಫಲಿತಾಂಶ || !ಮುಗಿದ)
        ಹೋಗು ಹೊರಹೋಗು;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, ದತ್ತಾಂಶ);
      FALLTHROUGH();

    ಸ್ಥಿತಿ H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT receive");
      ಫಲಿತಾಂಶ = recv_CONNECT_resp(cf, ದತ್ತಾಂಶ, ts, &ಮುಗಿದ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error receiving CONNECT response: %d",
                    (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);
      /* error or not complete yet. return for more multi-multi */
      ಆದರೆ(ಫಲಿತಾಂಶ || !ಮುಗಿದ)
        ಹೋಗು ಹೊರಹೋಗು;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, ದತ್ತಾಂಶ);
      FALLTHROUGH();

    ಸ್ಥಿತಿ H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT response");
      ಆದರೆ(ದತ್ತಾಂಶ->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
        ಆದರೆ(ts->close_connection || conn->ಬಿಟ್ಟುಗಳು.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "CONNECT need to close+open");
          infof(ದತ್ತಾಂಶ, "Connect me again please");
          ಹಿಂತಿರುಗಿ CURLE_AGAIN;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, ದತ್ತಾಂಶ);
        }
      }
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }

  } ಆಗ(ದತ್ತಾಂಶ->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  ಆದರೆ(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    ಆದರೆ(ದತ್ತಾಂಶ->info.httpproxycode / 100 != 2 &&
       ದತ್ತಾಂಶ->info.httpproxycode != 101) {
      curlx_safefree(ದತ್ತಾಂಶ->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ದತ್ತಾಂಶ);
      failf(ದತ್ತಾಂಶ, "CONNECT-UDP tunnel failed, response %d",
            ದತ್ತಾಂಶ->req.httpcode);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(ದತ್ತಾಂಶ->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(ದತ್ತಾಂಶ->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ದತ್ತಾಂಶ);
      failf(ದತ್ತಾಂಶ, "CONNECT tunnel failed, response %d", ದತ್ತಾಂಶ->req.httpcode);
      ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, ದತ್ತಾಂಶ);
  infof(ದತ್ತಾಂಶ, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", ದತ್ತಾಂಶ->info.httpproxycode);
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h1_proxy_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ cf_h1_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ h1_tunnel_state *ts = pctx->ts;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connect");
  ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!ts) {
    ಫಲಿತಾಂಶ = tunnel_init(cf, ದತ್ತಾಂಶ, &ts);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  ಫಲಿತಾಂಶ = H1_CONNECT(cf, ದತ್ತಾಂಶ, ts);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  curlx_safefree(ದತ್ತಾಂಶ->req.hd_proxy_auth);

ಹೊರಹೋಗು:
  *ಮುಗಿದ = (ಫಲಿತಾಂಶ == CURLE_OK) && tunnel_is_established(pctx->ts);
  ಆದರೆ(*ಮುಗಿದ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
    Curl_client_reset(ದತ್ತಾಂಶ);
    Curl_pgrsReset(ದತ್ತಾಂಶ);
    cf_tunnel_free(cf, ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h1_proxy_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_h1_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ h1_tunnel_state *ts = pctx->ts;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      ಆದರೆ(tunnel_want_send(ts))
        ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(ದತ್ತಾಂಶ, ps, sock);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಒಳ_only(ದತ್ತಾಂಶ, ps, sock);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(ದತ್ತಾಂಶ, ps, sock);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(cf->ಮುಂದಿನ)
      ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->adjust_pollset(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ps);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_h1_proxy_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                     ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ? cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) : FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h1_proxy_destroy(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  ಆದರೆ(pctx) {
    cf_tunnel_free(cf, ದತ್ತಾಂಶ);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ಸ್ಥಿರ CURLcode cf_h1_proxy_query(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_h1_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ h1_tunnel_state *ts = pctx ? pctx->ts : ಖಾಲಿ;
  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_HOST_ಪೋರ್ಟ್:
    ಆದರೆ(!ts || !ts->dest)
      ನಿಲ್ಲಿಸು;
    *pres1 = (ಪೂರ್ಣಾಂಕ)ts->dest->ಪೋರ್ಟ್;
    *((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)pres2) = ts->dest->hostname;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_ALPN_NEGOTIATED: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ರಚನೆ Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_peer *peer,
                                       ರಚನೆ Curl_peer *dest,
                                       ಪೂರ್ಣಾಂಕ httpversion,
                                       bool udp_tunnel)
{
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ cf_h1_proxy_ctx *pctx;
  ರಚನೆ h1_tunnel_state *ts;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(!dest)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ((httpversion < 10) || (httpversion >= 20))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ts = curlx_calloc(1, ಗಾತ್ರ(*ts));
  ಆದರೆ(!ts) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_ಶೀರ್ಷಿಕೆಗಳು);
  curlx_dyn_init(&ts->request_ದತ್ತಾಂಶ, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(ದತ್ತಾಂಶ, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, ಗಾತ್ರ(*pctx));
  ಆದರೆ(!pctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ts = ಖಾಲಿ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

ಹೊರಹೋಗು:
  tunnel_free(ts, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
