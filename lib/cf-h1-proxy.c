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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#ଅନ୍ତର୍ଭାଵ <curl/curl.h>
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "http1.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-h1-proxy.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
ସଂରଚନା h1_tunnel_state {
  ସଂରଚନା Curl_peer *dest;
  ସଂରଚନା dynbuf rcvbuf;
  ସଂରଚନା dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  ସଂରଚନା Curl_chunker ch;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion;
  ଗଣନା keeponval {
    KEEPON_DONE,
    KEEPON_CONNECT,
    KEEPON_IGNORE
  } keepon;
  curl_off_t cl; /* size of content to read and ignore */
  h1_tunnel_state tunnel_state;
  BIT(chunked_encoding);
  BIT(close_connection);
  BIT(maybe_folded);
  BIT(leading_unfold);
  BIT(udp_tunnel);
};

/* Persistent context for the H1-PROXY filter */
ସଂରଚନା cf_h1_proxy_ctx {
  ସଂରଚନା Curl_peer *peer;
  ସଂରଚନା h1_tunnel_state *ts;
};

ସ୍ଥିର bool tunnel_is_established(ସଂରଚନା h1_tunnel_state *ts)
{
  ଫେରନ୍ତୁ ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

ସ୍ଥିର bool tunnel_is_failed(ସଂରଚନା h1_tunnel_state *ts)
{
  ଫେରନ୍ତୁ ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

ସ୍ଥିର bool h1_proxy_is_udp(ସଂରଚନା Curl_cfilter *cf)
{
  ସଂରଚନା cf_h1_proxy_ctx *pctx = cf->ctx;
  ଫେରନ୍ତୁ (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

ସ୍ଥିର CURLcode tunnel_reinit(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା h1_tunnel_state *ts)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_data);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode tunnel_init(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା h1_tunnel_state **pts)
{
  ସଂରଚନା h1_tunnel_state *ts;

  ଯଦି(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, ଆକାର(*ts));
  ଯଦି(!ts)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  ଫେରନ୍ତୁ tunnel_reinit(cf, data, ts);
}

ସ୍ଥିର ଶୂନ୍ୟ h1_tunnel_go_state(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               ସଂରଚନା Curl_easy *data)
{
  ଯଦି(ts->tunnel_state == new_state)
    ଫେରନ୍ତୁ;
  /* entering this one */
  ବାଛ(new_state) {
  ସ୍ଥିତି H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ସ୍ଥିତି H1_TUNNEL_FAILED:
    ଯଦି(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_data);
    /* restore the protocol pointer */
    data->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ tunnel_free(ସଂରଚନା h1_tunnel_state *ts,
                        ସଂରଚନା Curl_easy *data)
{
  ଯଦି(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cf_tunnel_free(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data)
{
  ଯଦି(cf) {
    ସଂରଚନା cf_h1_proxy_ctx *pctx = cf->ctx;
    ସଂରଚନା h1_tunnel_state *ts = pctx ? pctx->ts : ଖାଲି;
    ଯଦି(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = ଖାଲି;
    }
  }
}

ସ୍ଥିର bool tunnel_want_send(ସଂରଚନା h1_tunnel_state *ts)
{
  ଫେରନ୍ତୁ ts->tunnel_state == H1_TUNNEL_CONNECT;
}

ସ୍ଥିର CURLcode start_CONNECT(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା h1_tunnel_state *ts)
{
  ସଂରଚନା cf_h1_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା httpreq *req = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  ଯଦି(result)
    ଯାଅ out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  ଯଦି(!result)
    result = Curl_creader_set_null(data);

out:
  ଯଦି(result)
    failf(data, "Failed sending CONNECT to proxy");
  ଯଦି(req)
    Curl_http_req_free(req);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode send_CONNECT(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା h1_tunnel_state *ts,
                             bool *done)
{
  ଅଚଳ uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ଯଦି(blen <= ts->nsent)
    ଯାଅ out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN)
      result = CURLE_OK;
    ଯାଅ out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (ଅଚଳ ଅକ୍ଷର *)buf, nwritten);

out:
  ଯଦି(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode on_resp_status(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା h1_tunnel_state *ts,
                               ଅଚଳ ଅକ୍ଷର *header)
{
  ସଂରଚନା SingleRequest *k = &data->req;

  (ଶୂନ୍ୟ)ts;
  ଯଦି(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  failf(data, "Invalid response header");
  ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
}

ସ୍ଥିର CURLcode on_resp_header(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା h1_tunnel_state *ts,
                               ଅଚଳ ଅକ୍ଷର *header)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  ଯଦି(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    ଅକ୍ଷର *auth = Curl_copy_header_ମୂଲ୍ୟ(header);
    ଯଦି(!auth)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_ନିବେଶ_auth(data, proxy, auth);

    curlx_free(auth);

    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(checkprefix("Content-Length:", header)) {
    ଯଦି(k->httpcode < 300) {
      ଯଦି(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    ଅନ୍ୟଥା {
      ଅଚଳ ଅକ୍ଷର *p = header + CURL_CSTRLEN("Content-Length:");
      ଯଦି(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  ଅନ୍ୟଥା ଯଦି(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(checkprefix("Transfer-Encoding:", header)) {
    ଯଦି(k->httpcode < 300) {
      ଯଦି(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    ଅନ୍ୟଥା ଯଦି(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  ଅନ୍ୟଥା ଯଦି(is_udp && checkprefix("Capsule-protocol:", header)) {
    ଯଦି(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  ଅନ୍ୟଥା ଯଦି(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode single_header(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t ଧାଡ଼ି_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ଅଚଳ ସଂରଚନା SingleRequest *k = &data->req;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, ଧାଡ଼ି_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, ଧାଡ଼ି_len);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, ଧାଡ଼ି_len);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = Curl_bump_headersize(data, ଧାଡ଼ି_len, TRUE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  ଯଦି(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    ଯଦି(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    ଅନ୍ୟଥା ଯଦି((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      ଯଦି(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      ଅନ୍ୟଥା ଯଦି(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      ଅନ୍ୟଥା {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    ଅନ୍ୟଥା {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    ଫେରନ୍ତୁ result;
  }

  ଯଦି(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  ଅନ୍ୟଥା
    result = on_resp_header(cf, data, ts, linep);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  curlx_dyn_reset(&ts->rcvbuf);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode recv_CONNECT_resp(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  ଯେତେବେଳେ(ts->keepon) {
    size_t nread;
    ଅକ୍ଷର byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    ଯଦି(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      ଫେରନ୍ତୁ CURLE_OK;

    ଯଦି(!result)
      result = Curl_pgrsUpdate(data);

    ଯଦି(result) {
      ts->keepon = KEEPON_DONE;
      ଭାଙ୍ଗ;
    }

    ଯଦି(!nread) {
      ଯଦି(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
        ts->maybe_folded = FALSE;
      }
      ଯଦି(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      ଅନ୍ୟଥା {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      ଭାଙ୍ଗ;
    }

    ଯଦି(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      ଯଦି(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
        ଯଦି(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      ଅନ୍ୟଥା ଯଦି(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        ଯଦି(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          ଭାଙ୍ଗ;
        }
      }
      ଜାରି;
    }

    ଯଦି(ts->maybe_folded) {
      ଯଦି(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      ଅନ୍ୟଥା {
        result = single_header(cf, data, ts);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    ଯଦି(ts->leading_unfold) {
      ଯଦି(ISBLANK(byte))
        /* skip a bit brother */
        ଜାରି;
      /* non-blank, insert a space then continue the unfolding */
      ଯଦି(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    ଯଦି(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    ଯଦି(byte != 0x0a)
      ଜାରି;
    ଅନ୍ୟଥା {
      ଅଚଳ ଅକ୍ଷର *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      ଯଦି(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
      ଅନ୍ୟଥା
        ts->maybe_folded = TRUE;
    }

    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  } /* while there is buffer left and loop is requested */

  ଯଦି(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  ଯଦି(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode H1_CONNECT(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା h1_tunnel_state *ts)
{
  ସଂରଚନା connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  ଯଦି(tunnel_is_established(ts))
    ଫେରନ୍ତୁ CURLE_OK;
  ଯଦି(tunnel_is_failed(ts))
    ଫେରନ୍ତୁ CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  କର {

    ଯଦି(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      ଯାଅ out;
    }

    ବାଛ(ts->tunnel_state) {
    ସ୍ଥିତି H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      ଯଦି(result)
        ଯାଅ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    ସ୍ଥିତି H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      ଯଦି(result || !done)
        ଯାଅ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    ସ୍ଥିତି H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      ଯଦି(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଯଦି(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      ଯଦି(result || !done)
        ଯାଅ out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    ସ୍ଥିତି H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      ଯଦି(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        ଯଦି(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          ଫେରନ୍ତୁ CURLE_AGAIN;
        }
        ଅନ୍ୟଥା {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }

  } ଯେତେବେଳେ(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  ଯଦି(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    ଯଦି(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }
  }
  ଅନ୍ୟଥା {
    ଯଦି(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  ଯଦି(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h1_proxy_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  ସଂରଚନା cf_h1_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା h1_tunnel_state *ts = pctx->ts;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  ଯଦି(result || !*done)
    ଫେରନ୍ତୁ result;

  *done = FALSE;
  ଯଦି(!ts) {
    result = tunnel_init(cf, data, &ts);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  ଯଦି(result)
    ଯାଅ out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  ଯଦି(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h1_proxy_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_h1_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  ଯଦି(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    ଯଦି(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      ଯଦି(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      ଅନ୍ୟଥା
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    ଅନ୍ୟଥା
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  ଅନ୍ୟଥା {
    ଯଦି(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_h1_proxy_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                     ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h1_proxy_destroy(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ଯଦି(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

ସ୍ଥିର CURLcode cf_h1_proxy_query(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_h1_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା h1_tunnel_state *ts = pctx ? pctx->ts : ଖାଲି;
  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_HOST_PORT:
    ଯଦି(!ts || !ts->dest)
      ଭାଙ୍ଗ;
    *pres1 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ts->dest->port;
    *((ଅଚଳ ଅକ୍ଷର **)pres2) = ts->dest->hostname;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_ALPN_NEGOTIATED: {
    ଅଚଳ ଅକ୍ଷର **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସଂରଚନା Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_peer *peer,
                                       ସଂରଚନା Curl_peer *dest,
                                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion,
                                       bool udp_tunnel)
{
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା cf_h1_proxy_ctx *pctx;
  ସଂରଚନା h1_tunnel_state *ts;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  ଯଦି(!dest)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି((httpversion < 10) || (httpversion >= 20))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ts = curlx_calloc(1, ଆକାର(*ts));
  ଯଦି(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, ଆକାର(*pctx));
  ଯଦି(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  ଯଦି(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    ଯାଅ out;
  }
  ts = ଖାଲି;
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
