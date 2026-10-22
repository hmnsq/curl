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

#ఒకవేళ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#అన్తర్భావ <curl/curl.h>
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "http1.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-h1-proxy.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "curlx/strparse.h"

రకనిర్వచనం గణన {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
నిర్మాణం h1_tunnel_state {
  నిర్మాణం Curl_peer *dest;
  నిర్మాణం dynbuf rcvbuf;
  నిర్మాణం dynbuf request_దత్తాంశం;
  size_t nsent;
  size_t headerlines;
  నిర్మాణం Curl_chunker ch;
  పూర్ణాంకం httpversion;
  గణన keeponval {
    KEEPON_పూర్తి,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  బిట్టు(chunked_encoding);
  బిట్టు(close_connection);
  బిట్టు(maybe_folded);
  బిట్టు(leading_unfold);
  బిట్టు(udp_tunnel);
};

/* Persistent context for the H1-PROXY filter */
నిర్మాణం cf_h1_proxy_ctx {
  నిర్మాణం Curl_peer *peer;
  నిర్మాణం h1_tunnel_state *ts;
};

స్థిర bool tunnel_is_established(నిర్మాణం h1_tunnel_state *ts)
{
  తిరిగి ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

స్థిర bool tunnel_is_failed(నిర్మాణం h1_tunnel_state *ts)
{
  తిరిగి ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

స్థిర bool h1_proxy_is_udp(నిర్మాణం Curl_cfilter *cf)
{
  నిర్మాణం cf_h1_proxy_ctx *pctx = cf->ctx;
  తిరిగి (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

స్థిర CURLcode tunnel_reinit(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం h1_tunnel_state *ts)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_దత్తాంశం);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode tunnel_init(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం h1_tunnel_state **pts)
{
  నిర్మాణం h1_tunnel_state *ts;

  ఒకవేళ(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(దత్తాంశం, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, పరిమాణం(*ts));
  ఒకవేళ(!ts)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  infof(దత్తాంశం, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_శీర్షికలు);
  curlx_dyn_init(&ts->request_దత్తాంశం, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(దత్తాంశం, &ts->ch, TRUE, TRUE);

  *pts = ts;
  తిరిగి tunnel_reinit(cf, దత్తాంశం, ts);
}

స్థిర శూన్య h1_tunnel_go_state(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(ts->tunnel_state == new_state)
    తిరిగి;
  /* entering this one */
  ఎంపిక(new_state) {
  స్థితి H1_TUNNEL_INIT:
    CURL_TRC_CF(దత్తాంశం, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, దత్తాంశం, ts);
    ఆపు;

  స్థితి H1_TUNNEL_CONNECT:
    CURL_TRC_CF(దత్తాంశం, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    ఆపు;

  స్థితి H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(దత్తాంశం, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    ఆపు;

  స్థితి H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(దత్తాంశం, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    ఆపు;

  స్థితి H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(దత్తాంశం, cf, "new tunnel state 'established'");
    infof(దత్తాంశం, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    దత్తాంశం->state.authproxy.పూర్తి = TRUE;
    దత్తాంశం->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  స్థితి H1_TUNNEL_FAILED:
    ఒకవేళ(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(దత్తాంశం, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_దత్తాంశం);
    /* restore the protocol pointer */
    దత్తాంశం->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(దత్తాంశం->req.hd_proxy_auth);
    ఆపు;
  }
}

స్థిర శూన్య tunnel_free(నిర్మాణం h1_tunnel_state *ts,
                        నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_దత్తాంశం);
    Curl_httpchunk_free(దత్తాంశం, &ts->ch);
    curlx_free(ts);
  }
}

స్థిర శూన్య cf_tunnel_free(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(cf) {
    నిర్మాణం cf_h1_proxy_ctx *pctx = cf->ctx;
    నిర్మాణం h1_tunnel_state *ts = pctx ? pctx->ts : ఖాళీ;
    ఒకవేళ(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, దత్తాంశం);
      tunnel_free(ts, దత్తాంశం);
      pctx->ts = ఖాళీ;
    }
  }
}

స్థిర bool tunnel_want_send(నిర్మాణం h1_tunnel_state *ts)
{
  తిరిగి ts->tunnel_state == H1_TUNNEL_CONNECT;
}

స్థిర CURLcode ప్రారంభం_CONNECT(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం h1_tunnel_state *ts)
{
  నిర్మాణం cf_h1_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం httpreq *req = ఖాళీ;
  పూర్ణాంకం http_minor;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(దత్తాంశం->req.newurl);

  ఫలితం = Curl_http_proxy_create_tunnel_request(&req, cf, దత్తాంశం,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  curlx_dyn_reset(&ts->request_దత్తాంశం);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  ఫలితం = Curl_h1_req_write_తల(req, http_minor, &ts->request_దత్తాంశం);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "Failed sending CONNECT to proxy");
  ఒకవేళ(req)
    Curl_http_req_free(req);
  తిరిగి ఫలితం;
}

స్థిర CURLcode send_CONNECT(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం h1_tunnel_state *ts,
                             bool *పూర్తి)
{
  స్థిరరాశి uint8_t *buf = curlx_dyn_uptr(&ts->request_దత్తాంశం);
  size_t request_len = curlx_dyn_len(&ts->request_దత్తాంశం);
  size_t blen = request_len;
  CURLcode ఫలితం = CURLE_OK;
  size_t nwritten;

  ఒకవేళ(blen <= ts->nsent)
    వెళ్ళు బహిర్గతం;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  ఫలితం = cf->తదుపరి->cft->do_send(cf->తదుపరి, దత్తాంశం, buf, blen, FALSE, &nwritten);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఫలితం = CURLE_OK;
    వెళ్ళు బహిర్గతం;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_బహిర్గతం, (స్థిరరాశి అక్షరం *)buf, nwritten);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "Failed sending CONNECT to proxy");
  *పూర్తి = (!ఫలితం && (ts->nsent >= request_len));
  తిరిగి ఫలితం;
}

స్థిర CURLcode on_resp_స్థితి(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం h1_tunnel_state *ts,
                               స్థిరరాశి అక్షరం *శీర్షిక)
{
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;

  (శూన్య)ts;
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(శీర్షిక, "HTTP/1.", 7) &&
          ((శీర్షిక[7] == '0') || (శీర్షిక[7] == '1')) &&
          (శీర్షిక[8] == ' ') &&
          ISDIGIT(శీర్షిక[9]) && ISDIGIT(శీర్షిక[10]) && ISDIGIT(శీర్షిక[11]) &&
          !ISDIGIT(శీర్షిక[12])) {
    /* store the HTTP code from the proxy */
    దత్తాంశం->info.httpproxycode = k->httpcode = ((శీర్షిక[9] - '0') * 100) +
      ((శీర్షిక[10] - '0') * 10) + (శీర్షిక[11] - '0');
    CURL_TRC_CF(దత్తాంశం, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    తిరిగి CURLE_OK;
  }
  failf(దత్తాంశం, "Invalid response header");
  తిరిగి CURLE_WEIRD_SERVER_జవాబు;
}

స్థిర CURLcode on_resp_శీర్షిక(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం h1_tunnel_state *ts,
                               స్థిరరాశి అక్షరం *శీర్షిక)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  bool is_udp = h1_proxy_is_udp(cf);

  ఒకవేళ(checkprefix("Proxy-authenticate:", శీర్షిక) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    అక్షరం *auth = Curl_copy_శీర్షిక_విలువ(శీర్షిక);
    ఒకవేళ(!auth)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    CURL_TRC_CF(దత్తాంశం, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", శీర్షిక);
    ఫలితం = Curl_http_ఆదానం_auth(దత్తాంశం, proxy, auth);

    curlx_free(auth);

    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(checkprefix("Content-Length:", శీర్షిక)) {
    ఒకవేళ(k->httpcode < 300) {
      ఒకవేళ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(దత్తాంశం, "Invalid Content-Length: in %03d response", k->httpcode);
        తిరిగి CURLE_WEIRD_SERVER_జవాబు;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(దత్తాంశం, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    లేకపోతే {
      స్థిరరాశి అక్షరం *p = శీర్షిక + CURL_CSTRLEN("Content-Length:");
      ఒకవేళ(curlx_str_numblanks(&p, &ts->cl)) {
        failf(దత్తాంశం, "Unsupported Content-Length value");
        తిరిగి CURLE_WEIRD_SERVER_జవాబు;
      }
    }
  }
  లేకపోతే ఒకవేళ(Curl_compareheader(శీర్షిక,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(దత్తాంశం, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  లేకపోతే ఒకవేళ(checkprefix("Transfer-Encoding:", శీర్షిక)) {
    ఒకవేళ(k->httpcode < 300) {
      ఒకవేళ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(దత్తాంశం, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        తిరిగి CURLE_WEIRD_SERVER_జవాబు;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(దత్తాంశం, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    లేకపోతే ఒకవేళ(Curl_compareheader(శీర్షిక,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(దత్తాంశం, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(దత్తాంశం, &ts->ch, TRUE);
    }
  }
  లేకపోతే ఒకవేళ(is_udp && checkprefix("Capsule-protocol:", శీర్షిక)) {
    ఒకవేళ(Curl_compareheader(శీర్షిక,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(దత్తాంశం, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  లేకపోతే ఒకవేళ(Curl_compareheader(శీర్షిక,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(దత్తాంశం, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode single_శీర్షిక(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం h1_tunnel_state *ts)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t పంక్తి_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  స్థిరరాశి నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  పూర్ణాంకం writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, linep, పంక్తి_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  ఫలితం = Curl_verify_శీర్షిక(దత్తాంశం, linep, పంక్తి_len);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* send the header to the callback */
  writetype = CLIENTWRITE_శీర్షిక | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_స్థితి : 0);
  ఫలితం = Curl_client_write(దత్తాంశం, writetype, linep, పంక్తి_len);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = Curl_bump_headersize(దత్తాంశం, పంక్తి_len, TRUE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  ఒకవేళ(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    ఒకవేళ(ts->headerlines == 1) {
      ts->keepon = KEEPON_పూర్తి;
      ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
    }
    లేకపోతే ఒకవేళ((407 == k->httpcode) && !దత్తాంశం->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      ఒకవేళ(ts->cl) {
        infof(దత్తాంశం, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      లేకపోతే ఒకవేళ(ts->chunked_encoding) {
        infof(దత్తాంశం, "Ignore chunked response-body");
      }
      లేకపోతే {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(దత్తాంశం, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_పూర్తి;
      }
    }
    లేకపోతే {
      ts->keepon = KEEPON_పూర్తి;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_పూర్తి);
    తిరిగి ఫలితం;
  }

  ఒకవేళ(ts->headerlines == 1)
    ఫలితం = on_resp_స్థితి(cf, దత్తాంశం, ts, linep);
  లేకపోతే
    ఫలితం = on_resp_శీర్షిక(cf, దత్తాంశం, ts, linep);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  curlx_dyn_reset(&ts->rcvbuf);
  తిరిగి ఫలితం;
}

స్థిర CURLcode recv_CONNECT_resp(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం h1_tunnel_state *ts,
                                  bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం error;

#నిర్వచించు SELECT_OK      0
#నిర్వచించు SELECT_ERROR   1

  error = SELECT_OK;
  *పూర్తి = FALSE;

  అయితే(ts->keepon) {
    size_t nread;
    అక్షరం byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    ఫలితం = Curl_conn_recv(దత్తాంశం, cf->sockindex, &byte, 1, &nread);
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      /* socket buffer drained, return */
      తిరిగి CURLE_OK;

    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_pgrsUpdate(దత్తాంశం);

    ఒకవేళ(ఫలితం) {
      ts->keepon = KEEPON_పూర్తి;
      ఆపు;
    }

    ఒకవేళ(!nread) {
      ఒకవేళ(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        ఫలితం = single_శీర్షిక(cf, దత్తాంశం, ts);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
        ts->maybe_folded = FALSE;
      }
      ఒకవేళ(దత్తాంశం->సమితి.proxyauth && దత్తాంశం->state.authproxy.avail &&
         దత్తాంశం->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(దత్తాంశం, "Proxy CONNECT connection closed");
      }
      లేకపోతే {
        error = SELECT_ERROR;
        failf(దత్తాంశం, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_పూర్తి;
      ఆపు;
    }

    ఒకవేళ(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      ఒకవేళ(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        ఫలితం = Curl_httpchunk_read(దత్తాంశం, &ts->ch, &byte, 1, &consumed);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
        ఒకవేళ(Curl_httpchunk_is_పూర్తి(దత్తాంశం, &ts->ch)) {
          /* we are done reading chunks! */
          infof(దత్తాంశం, "chunk reading DONE");
          ts->keepon = KEEPON_పూర్తి;
        }
      }
      లేకపోతే ఒకవేళ(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        ఒకవేళ(ts->cl <= 0) {
          ts->keepon = KEEPON_పూర్తి;
          ఆపు;
        }
      }
      కొనసాగు;
    }

    ఒకవేళ(ts->maybe_folded) {
      ఒకవేళ(ISBLANK(byte)) {
        Curl_http_గమ్యం_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      లేకపోతే {
        ఫలితం = single_శీర్షిక(cf, దత్తాంశం, ts);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    ఒకవేళ(ts->leading_unfold) {
      ఒకవేళ(ISBLANK(byte))
        /* skip a bit brother */
        కొనసాగు;
      /* non-blank, insert a space then continue the unfolding */
      ఒకవేళ(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(దత్తాంశం, "CONNECT response too large");
        తిరిగి CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    ఒకవేళ(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(దత్తాంశం, "CONNECT response too large");
      తిరిగి CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    ఒకవేళ(byte != 0x0a)
      కొనసాగు;
    లేకపోతే {
      స్థిరరాశి అక్షరం *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      ఒకవేళ(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        ఫలితం = single_శీర్షిక(cf, దత్తాంశం, ts);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
      లేకపోతే
        ts->maybe_folded = TRUE;
    }

    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  } /* while there is buffer left and loop is requested */

  ఒకవేళ(error)
    ఫలితం = CURLE_RECV_ERROR;
  *పూర్తి = (ts->keepon == KEEPON_పూర్తి);
  ఒకవేళ(!ఫలితం && *పూర్తి &&
     దత్తాంశం->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && దత్తాంశం->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    ఫలితం = Curl_http_auth_act(దత్తాంశం);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode H1_CONNECT(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం h1_tunnel_state *ts)
{
  నిర్మాణం connectdata *conn = cf->conn;
  CURLcode ఫలితం;
  bool పూర్తి;

  ఒకవేళ(tunnel_is_established(ts))
    తిరిగి CURLE_OK;
  ఒకవేళ(tunnel_is_failed(ts))
    తిరిగి CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  చేయి {

    ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
      failf(దత్తాంశం, "Proxy CONNECT aborted due to timeout");
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      వెళ్ళు బహిర్గతం;
    }

    ఎంపిక(ts->tunnel_state) {
    స్థితి H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(దత్తాంశం, cf, "CONNECT start");
      ఫలితం = ప్రారంభం_CONNECT(cf, దత్తాంశం, ts);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, దత్తాంశం);
      FALLTHROUGH();

    స్థితి H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(దత్తాంశం, cf, "CONNECT send");
      ఫలితం = send_CONNECT(cf, దత్తాంశం, ts, &పూర్తి);
      ఒకవేళ(ఫలితం || !పూర్తి)
        వెళ్ళు బహిర్గతం;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, దత్తాంశం);
      FALLTHROUGH();

    స్థితి H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(దత్తాంశం, cf, "CONNECT receive");
      ఫలితం = recv_CONNECT_resp(cf, దత్తాంశం, ts, &పూర్తి);
      ఒకవేళ(ఫలితం)
        CURL_TRC_CF(దత్తాంశం, cf, "error receiving CONNECT response: %d",
                    (పూర్ణాంకం)ఫలితం);
      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_pgrsUpdate(దత్తాంశం);
      /* error or not complete yet. return for more multi-multi */
      ఒకవేళ(ఫలితం || !పూర్తి)
        వెళ్ళు బహిర్గతం;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, దత్తాంశం);
      FALLTHROUGH();

    స్థితి H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(దత్తాంశం, cf, "CONNECT response");
      ఒకవేళ(దత్తాంశం->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&దత్తాంశం->req, దత్తాంశం);
        ఒకవేళ(ts->close_connection || conn->బిట్లు.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(దత్తాంశం, cf, "CONNECT need to close+open");
          infof(దత్తాంశం, "Connect me again please");
          తిరిగి CURLE_AGAIN;
        }
        లేకపోతే {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, దత్తాంశం);
        }
      }
      ఆపు;

    అప్రమేయ:
      ఆపు;
    }

  } అయితే(దత్తాంశం->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  ఒకవేళ(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    ఒకవేళ(దత్తాంశం->info.httpproxycode / 100 != 2 &&
       దత్తాంశం->info.httpproxycode != 101) {
      curlx_safefree(దత్తాంశం->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, దత్తాంశం);
      failf(దత్తాంశం, "CONNECT-UDP tunnel failed, response %d",
            దత్తాంశం->req.httpcode);
      తిరిగి CURLE_COULDNT_CONNECT;
    }
  }
  లేకపోతే {
    ఒకవేళ(దత్తాంశం->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(దత్తాంశం->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, దత్తాంశం);
      failf(దత్తాంశం, "CONNECT tunnel failed, response %d", దత్తాంశం->req.httpcode);
      తిరిగి CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, దత్తాంశం);
  infof(దత్తాంశం, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", దత్తాంశం->info.httpproxycode);
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, దత్తాంశం);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h1_proxy_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *పూర్తి)
{
  CURLcode ఫలితం;
  నిర్మాణం cf_h1_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం h1_tunnel_state *ts = pctx->ts;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "connect");
  ఫలితం = cf->తదుపరి->cft->do_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
  ఒకవేళ(ఫలితం || !*పూర్తి)
    తిరిగి ఫలితం;

  *పూర్తి = FALSE;
  ఒకవేళ(!ts) {
    ఫలితం = tunnel_init(cf, దత్తాంశం, &ts);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  ఫలితం = H1_CONNECT(cf, దత్తాంశం, ts);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  curlx_safefree(దత్తాంశం->req.hd_proxy_auth);

బహిర్గతం:
  *పూర్తి = (ఫలితం == CURLE_OK) && tunnel_is_established(pctx->ts);
  ఒకవేళ(*పూర్తి) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&దత్తాంశం->req, దత్తాంశం);
    Curl_client_reset(దత్తాంశం);
    Curl_pgrsReset(దత్తాంశం);
    cf_tunnel_free(cf, దత్తాంశం);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h1_proxy_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_h1_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం h1_tunnel_state *ts = pctx->ts;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, దత్తాంశం);
    ఒకవేళ(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      ఒకవేళ(tunnel_want_send(ts))
        ఫలితం = Curl_pollset_సమితి_బహిర్గతం_only(దత్తాంశం, ps, sock);
      లేకపోతే
        ఫలితం = Curl_pollset_సమితి_లోపల_only(దత్తాంశం, ps, sock);
    }
    లేకపోతే
      ఫలితం = Curl_pollset_సమితి_బహిర్గతం_only(దత్తాంశం, ps, sock);
  }
  లేకపోతే {
    ఒకవేళ(cf->తదుపరి)
      ఫలితం = cf->తదుపరి->cft->adjust_pollset(cf->తదుపరి, దత్తాంశం, ps);
  }
  తిరిగి ఫలితం;
}

స్థిర bool cf_h1_proxy_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                     స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి cf->తదుపరి ? cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం) : FALSE;
}

స్థిర శూన్య cf_h1_proxy_destroy(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  ఒకవేళ(pctx) {
    cf_tunnel_free(cf, దత్తాంశం);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

స్థిర CURLcode cf_h1_proxy_query(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_h1_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం h1_tunnel_state *ts = pctx ? pctx->ts : ఖాళీ;
  ఎంపిక(query) {
  స్థితి CF_QUERY_HOST_పోర్టు:
    ఒకవేళ(!ts || !ts->dest)
      ఆపు;
    *pres1 = (పూర్ణాంకం)ts->dest->పోర్టు;
    *((స్థిరరాశి అక్షరం **)pres2) = ts->dest->hostname;
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

నిర్మాణం Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_peer *peer,
                                       నిర్మాణం Curl_peer *dest,
                                       పూర్ణాంకం httpversion,
                                       bool udp_tunnel)
{
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం cf_h1_proxy_ctx *pctx;
  నిర్మాణం h1_tunnel_state *ts;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  ఒకవేళ(!dest)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ((httpversion < 10) || (httpversion >= 20))
    తిరిగి CURLE_FAILED_INIT;

  ts = curlx_calloc(1, పరిమాణం(*ts));
  ఒకవేళ(!ts) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_శీర్షికలు);
  curlx_dyn_init(&ts->request_దత్తాంశం, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(దత్తాంశం, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, పరిమాణం(*pctx));
  ఒకవేళ(!pctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  ఫలితం = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  ఒకవేళ(ఫలితం) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    వెళ్ళు బహిర్గతం;
  }
  ts = ఖాళీ;
  Curl_conn_cf_చొప్పించు_after(cf_at, cf);

బహిర్గతం:
  tunnel_free(ts, దత్తాంశం);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
