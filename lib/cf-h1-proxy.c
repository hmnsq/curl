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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#অন্তর্ভাব <curl/curl.h>
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "http1.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-h1-proxy.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curlx/strparse.h"

প্রকারনির্দেশ গণনা {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
সংরচনা h1_tunnel_state {
  সংরচনা Curl_peer *dest;
  সংরচনা dynbuf rcvbuf;
  সংরচনা dynbuf request_data;
  size_t nsent;
  size_t headerlines;
  সংরচনা Curl_chunker ch;
  পূর্ণাঙ্ক httpversion;
  গণনা keeponval {
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
সংরচনা cf_h1_proxy_ctx {
  সংরচনা Curl_peer *peer;
  সংরচনা h1_tunnel_state *ts;
};

স্থির bool tunnel_is_established(সংরচনা h1_tunnel_state *ts)
{
  প্রত্যাগম ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

স্থির bool tunnel_is_failed(সংরচনা h1_tunnel_state *ts)
{
  প্রত্যাগম ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

স্থির bool h1_proxy_is_udp(সংরচনা Curl_cfilter *cf)
{
  সংরচনা cf_h1_proxy_ctx *pctx = cf->ctx;
  প্রত্যাগম (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

স্থির CURLcode tunnel_reinit(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              সংরচনা h1_tunnel_state *ts)
{
  (শূন্য)data;
  (শূন্য)cf;
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
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode tunnel_init(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            সংরচনা h1_tunnel_state **pts)
{
  সংরচনা h1_tunnel_state *ts;

  যদি(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, পরিমাণ(*ts));
  যদি(!ts)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  প্রত্যাগম tunnel_reinit(cf, data, ts);
}

স্থির শূন্য h1_tunnel_go_state(সংরচনা Curl_cfilter *cf,
                               সংরচনা h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               সংরচনা Curl_easy *data)
{
  যদি(ts->tunnel_state == new_state)
    প্রত্যাগম;
  /* entering this one */
  বিকল্প(new_state) {
  পক্ষ H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    ভঙ্গ;

  পক্ষ H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    ভঙ্গ;

  পক্ষ H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    ভঙ্গ;

  পক্ষ H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    ভঙ্গ;

  পক্ষ H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  পক্ষ H1_TUNNEL_FAILED:
    যদি(new_state == H1_TUNNEL_FAILED)
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
    ভঙ্গ;
  }
}

স্থির শূন্য tunnel_free(সংরচনা h1_tunnel_state *ts,
                        সংরচনা Curl_easy *data)
{
  যদি(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

স্থির শূন্য cf_tunnel_free(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data)
{
  যদি(cf) {
    সংরচনা cf_h1_proxy_ctx *pctx = cf->ctx;
    সংরচনা h1_tunnel_state *ts = pctx ? pctx->ts : রিক্ত;
    যদি(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = রিক্ত;
    }
  }
}

স্থির bool tunnel_want_send(সংরচনা h1_tunnel_state *ts)
{
  প্রত্যাগম ts->tunnel_state == H1_TUNNEL_CONNECT;
}

স্থির CURLcode start_CONNECT(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              সংরচনা h1_tunnel_state *ts)
{
  সংরচনা cf_h1_proxy_ctx *pctx = cf->ctx;
  সংরচনা httpreq *req = রিক্ত;
  পূর্ণাঙ্ক http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  যদি(result)
    গচ্ছ out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  যদি(!result)
    result = Curl_creader_set_null(data);

out:
  যদি(result)
    failf(data, "Failed sending CONNECT to proxy");
  যদি(req)
    Curl_http_req_free(req);
  প্রত্যাগম result;
}

স্থির CURLcode send_CONNECT(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             সংরচনা h1_tunnel_state *ts,
                             bool *done)
{
  ধ্রুব uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  যদি(blen <= ts->nsent)
    গচ্ছ out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  যদি(result) {
    যদি(result == CURLE_AGAIN)
      result = CURLE_OK;
    গচ্ছ out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (ধ্রুব বর্ণ *)buf, nwritten);

out:
  যদি(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  প্রত্যাগম result;
}

স্থির CURLcode on_resp_status(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা h1_tunnel_state *ts,
                               ধ্রুব বর্ণ *header)
{
  সংরচনা SingleRequest *k = &data->req;

  (শূন্য)ts;
  যদি(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    প্রত্যাগম CURLE_OK;
  }
  failf(data, "Invalid response header");
  প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
}

স্থির CURLcode on_resp_header(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা h1_tunnel_state *ts,
                               ধ্রুব বর্ণ *header)
{
  CURLcode result = CURLE_OK;
  সংরচনা SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  যদি(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    বর্ণ *auth = Curl_copy_header_মান(header);
    যদি(!auth)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_প্রবেশ_auth(data, proxy, auth);

    curlx_free(auth);

    যদি(result)
      প্রত্যাগম result;
  }
  অন্যথা যদি(checkprefix("Content-Length:", header)) {
    যদি(k->httpcode < 300) {
      যদি(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    অন্যথা {
      ধ্রুব বর্ণ *p = header + CURL_CSTRLEN("Content-Length:");
      যদি(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  অন্যথা যদি(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  অন্যথা যদি(checkprefix("Transfer-Encoding:", header)) {
    যদি(k->httpcode < 300) {
      যদি(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        প্রত্যাগম CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    অন্যথা যদি(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  অন্যথা যদি(is_udp && checkprefix("Capsule-protocol:", header)) {
    যদি(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  অন্যথা যদি(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  প্রত্যাগম result;
}

স্থির CURLcode single_header(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              সংরচনা h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  ধ্রুব বর্ণ *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t সারি_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ধ্রুব সংরচনা SingleRequest *k = &data->req;
  পূর্ণাঙ্ক writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, সারি_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, সারি_len);
  যদি(result)
    প্রত্যাগম result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, সারি_len);
  যদি(result)
    প্রত্যাগম result;

  result = Curl_bump_headersize(data, সারি_len, TRUE);
  যদি(result)
    প্রত্যাগম result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  যদি(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    যদি(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    অন্যথা যদি((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      যদি(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      অন্যথা যদি(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      অন্যথা {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    অন্যথা {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    প্রত্যাগম result;
  }

  যদি(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  অন্যথা
    result = on_resp_header(cf, data, ts, linep);
  যদি(result)
    প্রত্যাগম result;

  curlx_dyn_reset(&ts->rcvbuf);
  প্রত্যাগম result;
}

স্থির CURLcode recv_CONNECT_resp(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  যাবত্(ts->keepon) {
    size_t nread;
    বর্ণ byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    যদি(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      প্রত্যাগম CURLE_OK;

    যদি(!result)
      result = Curl_pgrsUpdate(data);

    যদি(result) {
      ts->keepon = KEEPON_DONE;
      ভঙ্গ;
    }

    যদি(!nread) {
      যদি(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        যদি(result)
          প্রত্যাগম result;
        ts->maybe_folded = FALSE;
      }
      যদি(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      অন্যথা {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      ভঙ্গ;
    }

    যদি(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      যদি(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        যদি(result)
          প্রত্যাগম result;
        যদি(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      অন্যথা যদি(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        যদি(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          ভঙ্গ;
        }
      }
      অনুবর্ত;
    }

    যদি(ts->maybe_folded) {
      যদি(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      অন্যথা {
        result = single_header(cf, data, ts);
        যদি(result)
          প্রত্যাগম result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    যদি(ts->leading_unfold) {
      যদি(ISBLANK(byte))
        /* skip a bit brother */
        অনুবর্ত;
      /* non-blank, insert a space then continue the unfolding */
      যদি(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        প্রত্যাগম CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    যদি(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      প্রত্যাগম CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    যদি(byte != 0x0a)
      অনুবর্ত;
    অন্যথা {
      ধ্রুব বর্ণ *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      যদি(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        যদি(result)
          প্রত্যাগম result;
      }
      অন্যথা
        ts->maybe_folded = TRUE;
    }

    যদি(result)
      প্রত্যাগম result;
  } /* while there is buffer left and loop is requested */

  যদি(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  যদি(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  প্রত্যাগম result;
}

স্থির CURLcode H1_CONNECT(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           সংরচনা h1_tunnel_state *ts)
{
  সংরচনা connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  যদি(tunnel_is_established(ts))
    প্রত্যাগম CURLE_OK;
  যদি(tunnel_is_failed(ts))
    প্রত্যাগম CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  কুরু {

    যদি(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      গচ্ছ out;
    }

    বিকল্প(ts->tunnel_state) {
    পক্ষ H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      যদি(result)
        গচ্ছ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    পক্ষ H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      যদি(result || !done)
        গচ্ছ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    পক্ষ H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      যদি(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (পূর্ণাঙ্ক)result);
      যদি(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      যদি(result || !done)
        গচ্ছ out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    পক্ষ H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      যদি(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        যদি(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          প্রত্যাগম CURLE_AGAIN;
        }
        অন্যথা {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      ভঙ্গ;

    স্বভাব:
      ভঙ্গ;
    }

  } যাবত্(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  যদি(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    যদি(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }
  }
  অন্যথা {
    যদি(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      প্রত্যাগম CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  যদি(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  প্রত্যাগম result;
}

স্থির CURLcode cf_h1_proxy_connect(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  সংরচনা cf_h1_proxy_ctx *pctx = cf->ctx;
  সংরচনা h1_tunnel_state *ts = pctx->ts;

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  যদি(result || !*done)
    প্রত্যাগম result;

  *done = FALSE;
  যদি(!ts) {
    result = tunnel_init(cf, data, &ts);
    যদি(result)
      প্রত্যাগম result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  যদি(result)
    গচ্ছ out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  যদি(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  প্রত্যাগম result;
}

স্থির CURLcode cf_h1_proxy_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                           সংরচনা Curl_easy *data,
                                           সংরচনা easy_pollset *ps)
{
  সংরচনা cf_h1_proxy_ctx *pctx = cf->ctx;
  সংরচনা h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  যদি(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    যদি(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      যদি(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      অন্যথা
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    অন্যথা
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  অন্যথা {
    যদি(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  প্রত্যাগম result;
}

স্থির bool cf_h1_proxy_data_pending(সংরচনা Curl_cfilter *cf,
                                     ধ্রুব সংরচনা Curl_easy *data)
{
  প্রত্যাগম cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

স্থির শূন্য cf_h1_proxy_destroy(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data)
{
  সংরচনা cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  যদি(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

স্থির CURLcode cf_h1_proxy_query(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_h1_proxy_ctx *pctx = cf->ctx;
  সংরচনা h1_tunnel_state *ts = pctx ? pctx->ts : রিক্ত;
  বিকল্প(query) {
  পক্ষ CF_QUERY_HOST_PORT:
    যদি(!ts || !ts->dest)
      ভঙ্গ;
    *pres1 = (পূর্ণাঙ্ক)ts->dest->port;
    *((ধ্রুব বর্ণ **)pres2) = ts->dest->hostname;
    প্রত্যাগম CURLE_OK;
  পক্ষ CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুব বর্ণ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = রিক্ত;
    প্রত্যাগম CURLE_OK;
  }
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

সংরচনা Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা Curl_peer *peer,
                                       সংরচনা Curl_peer *dest,
                                       পূর্ণাঙ্ক httpversion,
                                       bool udp_tunnel)
{
  সংরচনা Curl_cfilter *cf;
  সংরচনা cf_h1_proxy_ctx *pctx;
  সংরচনা h1_tunnel_state *ts;
  CURLcode result;

  (শূন্য)data;
  যদি(!dest)
    প্রত্যাগম CURLE_FAILED_INIT;
  যদি((httpversion < 10) || (httpversion >= 20))
    প্রত্যাগম CURLE_FAILED_INIT;

  ts = curlx_calloc(1, পরিমাণ(*ts));
  যদি(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, পরিমাণ(*pctx));
  যদি(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  যদি(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    গচ্ছ out;
  }
  ts = রিক্ত;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  প্রত্যাগম result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
