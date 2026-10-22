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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#അന്തര്ഭാവ <curl/curl.h>
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "http1.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-h1-proxy.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "curlx/strparse.h"

തരനിർവചനം എണ്ണൽ {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
ഘടന h1_tunnel_state {
  ഘടന Curl_peer *dest;
  ഘടന dynbuf rcvbuf;
  ഘടന dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  ഘടന Curl_chunker ch;
  പൂർണ്ണസംഖ്യ httpversion;
  എണ്ണൽ keeponval {
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
ഘടന cf_h1_proxy_ctx {
  ഘടന Curl_peer *peer;
  ഘടന h1_tunnel_state *ts;
};

സ്ഥിതം bool tunnel_is_established(ഘടന h1_tunnel_state *ts)
{
  തിരികെ ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

സ്ഥിതം bool tunnel_is_failed(ഘടന h1_tunnel_state *ts)
{
  തിരികെ ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

സ്ഥിതം bool h1_proxy_is_udp(ഘടന Curl_cfilter *cf)
{
  ഘടന cf_h1_proxy_ctx *pctx = cf->ctx;
  തിരികെ (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

സ്ഥിതം CURLcode tunnel_reinit(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              ഘടന h1_tunnel_state *ts)
{
  (ശൂന്യം)data;
  (ശൂന്യം)cf;
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
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode tunnel_init(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            ഘടന h1_tunnel_state **pts)
{
  ഘടന h1_tunnel_state *ts;

  എങ്കിൽ(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, വലുപ്പം(*ts));
  എങ്കിൽ(!ts)
    തിരികെ CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  തിരികെ tunnel_reinit(cf, data, ts);
}

സ്ഥിതം ശൂന്യം h1_tunnel_go_state(ഘടന Curl_cfilter *cf,
                               ഘടന h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               ഘടന Curl_easy *data)
{
  എങ്കിൽ(ts->tunnel_state == new_state)
    തിരികെ;
  /* entering this one */
  തിരഞ്ഞെടുപ്പ്(new_state) {
  അവസ്ഥ H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    നിർത്തുക;

  അവസ്ഥ H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    നിർത്തുക;

  അവസ്ഥ H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    നിർത്തുക;

  അവസ്ഥ H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    നിർത്തുക;

  അവസ്ഥ H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  അവസ്ഥ H1_TUNNEL_FAILED:
    എങ്കിൽ(new_state == H1_TUNNEL_FAILED)
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
    നിർത്തുക;
  }
}

സ്ഥിതം ശൂന്യം tunnel_free(ഘടന h1_tunnel_state *ts,
                        ഘടന Curl_easy *data)
{
  എങ്കിൽ(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

സ്ഥിതം ശൂന്യം cf_tunnel_free(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data)
{
  എങ്കിൽ(cf) {
    ഘടന cf_h1_proxy_ctx *pctx = cf->ctx;
    ഘടന h1_tunnel_state *ts = pctx ? pctx->ts : ശൂന്യ;
    എങ്കിൽ(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = ശൂന്യ;
    }
  }
}

സ്ഥിതം bool tunnel_want_send(ഘടന h1_tunnel_state *ts)
{
  തിരികെ ts->tunnel_state == H1_TUNNEL_CONNECT;
}

സ്ഥിതം CURLcode start_CONNECT(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              ഘടന h1_tunnel_state *ts)
{
  ഘടന cf_h1_proxy_ctx *pctx = cf->ctx;
  ഘടന httpreq *req = ശൂന്യ;
  പൂർണ്ണസംഖ്യ http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  എങ്കിൽ(result)
    പോകുക out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  എങ്കിൽ(!result)
    result = Curl_creader_set_null(data);

out:
  എങ്കിൽ(result)
    failf(data, "Failed sending CONNECT to proxy");
  എങ്കിൽ(req)
    Curl_http_req_free(req);
  തിരികെ result;
}

സ്ഥിതം CURLcode send_CONNECT(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             ഘടന h1_tunnel_state *ts,
                             bool *done)
{
  സ്ഥിരം uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  എങ്കിൽ(blen <= ts->nsent)
    പോകുക out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN)
      result = CURLE_OK;
    പോകുക out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (സ്ഥിരം അക്ഷരം *)buf, nwritten);

out:
  എങ്കിൽ(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  തിരികെ result;
}

സ്ഥിതം CURLcode on_resp_status(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന h1_tunnel_state *ts,
                               സ്ഥിരം അക്ഷരം *header)
{
  ഘടന SingleRequest *k = &data->req;

  (ശൂന്യം)ts;
  എങ്കിൽ(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    തിരികെ CURLE_OK;
  }
  failf(data, "Invalid response header");
  തിരികെ CURLE_WEIRD_SERVER_REPLY;
}

സ്ഥിതം CURLcode on_resp_header(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന h1_tunnel_state *ts,
                               സ്ഥിരം അക്ഷരം *header)
{
  CURLcode result = CURLE_OK;
  ഘടന SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  എങ്കിൽ(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    അക്ഷരം *auth = Curl_copy_header_മൂല്യം(header);
    എങ്കിൽ(!auth)
      തിരികെ CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_നിവേശം_auth(data, proxy, auth);

    curlx_free(auth);

    എങ്കിൽ(result)
      തിരികെ result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("Content-Length:", header)) {
    എങ്കിൽ(k->httpcode < 300) {
      എങ്കിൽ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        തിരികെ CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    അല്ലെങ്കിൽ {
      സ്ഥിരം അക്ഷരം *p = header + CURL_CSTRLEN("Content-Length:");
      എങ്കിൽ(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        തിരികെ CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("Transfer-Encoding:", header)) {
    എങ്കിൽ(k->httpcode < 300) {
      എങ്കിൽ(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        തിരികെ CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(is_udp && checkprefix("Capsule-protocol:", header)) {
    എങ്കിൽ(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode single_header(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              ഘടന h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t വരി_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  സ്ഥിരം ഘടന SingleRequest *k = &data->req;
  പൂർണ്ണസംഖ്യ writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, വരി_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, വരി_len);
  എങ്കിൽ(result)
    തിരികെ result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, വരി_len);
  എങ്കിൽ(result)
    തിരികെ result;

  result = Curl_bump_headersize(data, വരി_len, TRUE);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  എങ്കിൽ(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    എങ്കിൽ(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      എങ്കിൽ(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      അല്ലെങ്കിൽ {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    അല്ലെങ്കിൽ {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    തിരികെ result;
  }

  എങ്കിൽ(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  അല്ലെങ്കിൽ
    result = on_resp_header(cf, data, ts, linep);
  എങ്കിൽ(result)
    തിരികെ result;

  curlx_dyn_reset(&ts->rcvbuf);
  തിരികെ result;
}

സ്ഥിതം CURLcode recv_CONNECT_resp(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  ആകുമ്പോൾ(ts->keepon) {
    size_t nread;
    അക്ഷരം byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    എങ്കിൽ(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      തിരികെ CURLE_OK;

    എങ്കിൽ(!result)
      result = Curl_pgrsUpdate(data);

    എങ്കിൽ(result) {
      ts->keepon = KEEPON_DONE;
      നിർത്തുക;
    }

    എങ്കിൽ(!nread) {
      എങ്കിൽ(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        എങ്കിൽ(result)
          തിരികെ result;
        ts->maybe_folded = FALSE;
      }
      എങ്കിൽ(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      അല്ലെങ്കിൽ {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      നിർത്തുക;
    }

    എങ്കിൽ(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      എങ്കിൽ(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        എങ്കിൽ(result)
          തിരികെ result;
        എങ്കിൽ(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      അല്ലെങ്കിൽ എങ്കിൽ(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        എങ്കിൽ(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          നിർത്തുക;
        }
      }
      തുടരുക;
    }

    എങ്കിൽ(ts->maybe_folded) {
      എങ്കിൽ(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      അല്ലെങ്കിൽ {
        result = single_header(cf, data, ts);
        എങ്കിൽ(result)
          തിരികെ result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    എങ്കിൽ(ts->leading_unfold) {
      എങ്കിൽ(ISBLANK(byte))
        /* skip a bit brother */
        തുടരുക;
      /* non-blank, insert a space then continue the unfolding */
      എങ്കിൽ(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        തിരികെ CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    എങ്കിൽ(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      തിരികെ CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    എങ്കിൽ(byte != 0x0a)
      തുടരുക;
    അല്ലെങ്കിൽ {
      സ്ഥിരം അക്ഷരം *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      എങ്കിൽ(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        എങ്കിൽ(result)
          തിരികെ result;
      }
      അല്ലെങ്കിൽ
        ts->maybe_folded = TRUE;
    }

    എങ്കിൽ(result)
      തിരികെ result;
  } /* while there is buffer left and loop is requested */

  എങ്കിൽ(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  എങ്കിൽ(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode H1_CONNECT(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           ഘടന h1_tunnel_state *ts)
{
  ഘടന connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  എങ്കിൽ(tunnel_is_established(ts))
    തിരികെ CURLE_OK;
  എങ്കിൽ(tunnel_is_failed(ts))
    തിരികെ CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  ചെയ്യുക {

    എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      പോകുക out;
    }

    തിരഞ്ഞെടുപ്പ്(ts->tunnel_state) {
    അവസ്ഥ H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      എങ്കിൽ(result)
        പോകുക out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    അവസ്ഥ H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      എങ്കിൽ(result || !done)
        പോകുക out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    അവസ്ഥ H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      എങ്കിൽ(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (പൂർണ്ണസംഖ്യ)result);
      എങ്കിൽ(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      എങ്കിൽ(result || !done)
        പോകുക out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    അവസ്ഥ H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      എങ്കിൽ(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        എങ്കിൽ(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          തിരികെ CURLE_AGAIN;
        }
        അല്ലെങ്കിൽ {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }

  } ആകുമ്പോൾ(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  എങ്കിൽ(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    എങ്കിൽ(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      തിരികെ CURLE_COULDNT_CONNECT;
    }
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      തിരികെ CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  എങ്കിൽ(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h1_proxy_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  ഘടന cf_h1_proxy_ctx *pctx = cf->ctx;
  ഘടന h1_tunnel_state *ts = pctx->ts;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  എങ്കിൽ(result || !*done)
    തിരികെ result;

  *done = FALSE;
  എങ്കിൽ(!ts) {
    result = tunnel_init(cf, data, &ts);
    എങ്കിൽ(result)
      തിരികെ result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  എങ്കിൽ(result)
    പോകുക out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  എങ്കിൽ(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h1_proxy_adjust_pollset(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data,
                                           ഘടന easy_pollset *ps)
{
  ഘടന cf_h1_proxy_ctx *pctx = cf->ctx;
  ഘടന h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    എങ്കിൽ(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      എങ്കിൽ(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      അല്ലെങ്കിൽ
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    അല്ലെങ്കിൽ
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  തിരികെ result;
}

സ്ഥിതം bool cf_h1_proxy_data_pending(ഘടന Curl_cfilter *cf,
                                     സ്ഥിരം ഘടന Curl_easy *data)
{
  തിരികെ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

സ്ഥിതം ശൂന്യം cf_h1_proxy_destroy(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data)
{
  ഘടന cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  എങ്കിൽ(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

സ്ഥിതം CURLcode cf_h1_proxy_query(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_h1_proxy_ctx *pctx = cf->ctx;
  ഘടന h1_tunnel_state *ts = pctx ? pctx->ts : ശൂന്യ;
  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_HOST_PORT:
    എങ്കിൽ(!ts || !ts->dest)
      നിർത്തുക;
    *pres1 = (പൂർണ്ണസംഖ്യ)ts->dest->port;
    *((സ്ഥിരം അക്ഷരം **)pres2) = ts->dest->hostname;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_ALPN_NEGOTIATED: {
    സ്ഥിരം അക്ഷരം **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ശൂന്യ;
    തിരികെ CURLE_OK;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ഘടന Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                       ഘടന Curl_easy *data,
                                       ഘടന Curl_peer *peer,
                                       ഘടന Curl_peer *dest,
                                       പൂർണ്ണസംഖ്യ httpversion,
                                       bool udp_tunnel)
{
  ഘടന Curl_cfilter *cf;
  ഘടന cf_h1_proxy_ctx *pctx;
  ഘടന h1_tunnel_state *ts;
  CURLcode result;

  (ശൂന്യം)data;
  എങ്കിൽ(!dest)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ((httpversion < 10) || (httpversion >= 20))
    തിരികെ CURLE_FAILED_INIT;

  ts = curlx_calloc(1, വലുപ്പം(*ts));
  എങ്കിൽ(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, വലുപ്പം(*pctx));
  എങ്കിൽ(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  എങ്കിൽ(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    പോകുക out;
  }
  ts = ശൂന്യ;
  Curl_conn_cf_തിരുകുക_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  തിരികെ result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
