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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#समावेश <curl/curl.h>
#समावेश "urldata.h"
#समावेश "curlx/dynbuf.h"
#समावेश "sendf.h"
#समावेश "http.h"
#समावेश "http1.h"
#समावेश "http_proxy.h"
#समावेश "select.h"
#समावेश "progress.h"
#समावेश "multiif.h"
#समावेश "cfilters.h"
#समावेश "cf-h1-proxy.h"
#समावेश "connect.h"
#समावेश "curl_trc.h"
#समावेश "strcase.h"
#समावेश "curlx/strparse.h"

प्रकार क्रमागत {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
काष्ठा h1_tunnel_state {
  काष्ठा Curl_peer *dest;
  काष्ठा dynbuf rcvbuf;
  काष्ठा dynbuf request_तथ्य;
  माप_प्रकार nsent;
  माप_प्रकार headerlines;
  काष्ठा Curl_chunker ch;
  पूर्णांक httpversion;
  क्रमागत keeponval {
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
काष्ठा cf_h1_proxy_ctx {
  काष्ठा Curl_peer *peer;
  काष्ठा h1_tunnel_state *ts;
};

अटल bool tunnel_is_established(काष्ठा h1_tunnel_state *ts)
{
  वापस ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

अटल bool tunnel_is_failed(काष्ठा h1_tunnel_state *ts)
{
  वापस ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

अटल bool h1_proxy_is_udp(काष्ठा Curl_cfilter *cf)
{
  काष्ठा cf_h1_proxy_ctx *pctx = cf->ctx;
  वापस (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

अटल CURLcode tunnel_reinit(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा h1_tunnel_state *ts)
{
  (व्योम)तथ्य;
  (व्योम)cf;
  DEBUGASSERT(ts);
  curlx_dyn_reset(&ts->rcvbuf);
  curlx_dyn_reset(&ts->request_तथ्य);
  ts->tunnel_state = H1_TUNNEL_INIT;
  ts->keepon = KEEPON_CONNECT;
  ts->cl = 0;
  ts->close_connection = FALSE;
  ts->maybe_folded = FALSE;
  ts->leading_unfold = FALSE;
  ts->chunked_encoding = FALSE;
  ts->nsent = 0;
  ts->headerlines = 0;
  वापस CURLE_OK;
}

अटल CURLcode tunnel_init(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा h1_tunnel_state **pts)
{
  काष्ठा h1_tunnel_state *ts;

  अगर(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(तथ्य, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, माप(*ts));
  अगर(!ts)
    वापस CURLE_OUT_OF_MEMORY;

  infof(तथ्य, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_तथ्य, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(तथ्य, &ts->ch, TRUE, TRUE);

  *pts = ts;
  वापस tunnel_reinit(cf, तथ्य, ts);
}

अटल व्योम h1_tunnel_go_state(काष्ठा Curl_cfilter *cf,
                               काष्ठा h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               काष्ठा Curl_easy *तथ्य)
{
  अगर(ts->tunnel_state == new_state)
    वापस;
  /* entering this one */
  चयन(new_state) {
  हाल H1_TUNNEL_INIT:
    CURL_TRC_CF(तथ्य, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, तथ्य, ts);
    अवरोध;

  हाल H1_TUNNEL_CONNECT:
    CURL_TRC_CF(तथ्य, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    अवरोध;

  हाल H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(तथ्य, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    अवरोध;

  हाल H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(तथ्य, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    अवरोध;

  हाल H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(तथ्य, cf, "new tunnel state 'established'");
    infof(तथ्य, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    तथ्य->state.authproxy.done = TRUE;
    तथ्य->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  हाल H1_TUNNEL_FAILED:
    अगर(new_state == H1_TUNNEL_FAILED)
      CURL_TRC_CF(तथ्य, cf, "new tunnel state 'failed'");
    ts->tunnel_state = new_state;
    curlx_dyn_reset(&ts->rcvbuf);
    curlx_dyn_reset(&ts->request_तथ्य);
    /* restore the protocol pointer */
    तथ्य->info.httpcode = 0; /* clear it as it might have been used for the
                                proxy */
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(तथ्य->req.hd_proxy_auth);
    अवरोध;
  }
}

अटल व्योम tunnel_free(काष्ठा h1_tunnel_state *ts,
                        काष्ठा Curl_easy *तथ्य)
{
  अगर(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_तथ्य);
    Curl_httpchunk_free(तथ्य, &ts->ch);
    curlx_free(ts);
  }
}

अटल व्योम cf_tunnel_free(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य)
{
  अगर(cf) {
    काष्ठा cf_h1_proxy_ctx *pctx = cf->ctx;
    काष्ठा h1_tunnel_state *ts = pctx ? pctx->ts : शून्य;
    अगर(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, तथ्य);
      tunnel_free(ts, तथ्य);
      pctx->ts = शून्य;
    }
  }
}

अटल bool tunnel_want_send(काष्ठा h1_tunnel_state *ts)
{
  वापस ts->tunnel_state == H1_TUNNEL_CONNECT;
}

अटल CURLcode start_CONNECT(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा h1_tunnel_state *ts)
{
  काष्ठा cf_h1_proxy_ctx *pctx = cf->ctx;
  काष्ठा httpreq *req = शून्य;
  पूर्णांक http_minor;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(तथ्य->req.newurl);

  परिणाम = Curl_http_proxy_create_tunnel_request(&req, cf, तथ्य,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  अगर(परिणाम)
    जाओ out;

  curlx_dyn_reset(&ts->request_तथ्य);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  परिणाम = Curl_h1_req_write_शीर्ष(req, http_minor, &ts->request_तथ्य);
  अगर(!परिणाम)
    परिणाम = Curl_creader_set_null(तथ्य);

out:
  अगर(परिणाम)
    failf(तथ्य, "Failed sending CONNECT to proxy");
  अगर(req)
    Curl_http_req_free(req);
  वापस परिणाम;
}

अटल CURLcode send_CONNECT(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा h1_tunnel_state *ts,
                             bool *done)
{
  स्थिर uint8_t *buf = curlx_dyn_uptr(&ts->request_तथ्य);
  माप_प्रकार request_len = curlx_dyn_len(&ts->request_तथ्य);
  माप_प्रकार blen = request_len;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nwritten;

  अगर(blen <= ts->nsent)
    जाओ out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  परिणाम = cf->अग्र->cft->do_send(cf->अग्र, तथ्य, buf, blen, FALSE, &nwritten);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
    जाओ out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(तथ्य, CURLINFO_HEADER_OUT, (स्थिर अक्षर *)buf, nwritten);

out:
  अगर(परिणाम)
    failf(तथ्य, "Failed sending CONNECT to proxy");
  *done = (!परिणाम && (ts->nsent >= request_len));
  वापस परिणाम;
}

अटल CURLcode on_resp_status(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा h1_tunnel_state *ts,
                               स्थिर अक्षर *header)
{
  काष्ठा SingleRequest *k = &तथ्य->req;

  (व्योम)ts;
  अगर(!म_भेदन(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    तथ्य->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(तथ्य, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    वापस CURLE_OK;
  }
  failf(तथ्य, "Invalid response header");
  वापस CURLE_WEIRD_SERVER_REPLY;
}

अटल CURLcode on_resp_header(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा h1_tunnel_state *ts,
                               स्थिर अक्षर *header)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा SingleRequest *k = &तथ्य->req;
  bool is_udp = h1_proxy_is_udp(cf);

  अगर(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    अक्षर *auth = Curl_copy_header_मूल्य(header);
    अगर(!auth)
      वापस CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(तथ्य, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    परिणाम = Curl_http_निवेश_auth(तथ्य, proxy, auth);

    curlx_free(auth);

    अगर(परिणाम)
      वापस परिणाम;
  }
  अन्यथा अगर(checkprefix("Content-Length:", header)) {
    अगर(k->httpcode < 300) {
      अगर(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(तथ्य, "Invalid Content-Length: in %03d response", k->httpcode);
        वापस CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(तथ्य, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    अन्यथा {
      स्थिर अक्षर *p = header + CURL_CSTRLEN("Content-Length:");
      अगर(curlx_str_numblanks(&p, &ts->cl)) {
        failf(तथ्य, "Unsupported Content-Length value");
        वापस CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  अन्यथा अगर(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(तथ्य, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  अन्यथा अगर(checkprefix("Transfer-Encoding:", header)) {
    अगर(k->httpcode < 300) {
      अगर(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(तथ्य, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        वापस CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(तथ्य, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    अन्यथा अगर(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(तथ्य, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(तथ्य, &ts->ch, TRUE);
    }
  }
  अन्यथा अगर(is_udp && checkprefix("Capsule-protocol:", header)) {
    अगर(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(तथ्य, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  अन्यथा अगर(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(तथ्य, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  वापस परिणाम;
}

अटल CURLcode single_header(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा h1_tunnel_state *ts)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *linep = curlx_dyn_ptr(&ts->rcvbuf);
  माप_प्रकार पंक्ति_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  स्थिर काष्ठा SingleRequest *k = &तथ्य->req;
  पूर्णांक writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(तथ्य, CURLINFO_HEADER_IN, linep, पंक्ति_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  परिणाम = Curl_verify_header(तथ्य, linep, पंक्ति_len);
  अगर(परिणाम)
    वापस परिणाम;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  परिणाम = Curl_client_write(तथ्य, writetype, linep, पंक्ति_len);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = Curl_bump_headersize(तथ्य, पंक्ति_len, TRUE);
  अगर(परिणाम)
    वापस परिणाम;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  अगर(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    अगर(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      परिणाम = CURLE_WEIRD_SERVER_REPLY;
    }
    अन्यथा अगर((407 == k->httpcode) && !तथ्य->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      अगर(ts->cl) {
        infof(तथ्य, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      अन्यथा अगर(ts->chunked_encoding) {
        infof(तथ्य, "Ignore chunked response-body");
      }
      अन्यथा {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(तथ्य, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    अन्यथा {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    वापस परिणाम;
  }

  अगर(ts->headerlines == 1)
    परिणाम = on_resp_status(cf, तथ्य, ts, linep);
  अन्यथा
    परिणाम = on_resp_header(cf, तथ्य, ts, linep);
  अगर(परिणाम)
    वापस परिणाम;

  curlx_dyn_reset(&ts->rcvbuf);
  वापस परिणाम;
}

अटल CURLcode recv_CONNECT_resp(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक error;

#घोषणा SELECT_OK      0
#घोषणा SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  जबतक(ts->keepon) {
    माप_प्रकार nread;
    अक्षर byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    परिणाम = Curl_conn_recv(तथ्य, cf->sockindex, &byte, 1, &nread);
    अगर(परिणाम == CURLE_AGAIN)
      /* socket buffer drained, return */
      वापस CURLE_OK;

    अगर(!परिणाम)
      परिणाम = Curl_pgrsUpdate(तथ्य);

    अगर(परिणाम) {
      ts->keepon = KEEPON_DONE;
      अवरोध;
    }

    अगर(!nread) {
      अगर(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        परिणाम = single_header(cf, तथ्य, ts);
        अगर(परिणाम)
          वापस परिणाम;
        ts->maybe_folded = FALSE;
      }
      अगर(तथ्य->set.proxyauth && तथ्य->state.authproxy.avail &&
         तथ्य->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(तथ्य, "Proxy CONNECT connection closed");
      }
      अन्यथा {
        error = SELECT_ERROR;
        failf(तथ्य, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      अवरोध;
    }

    अगर(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      अगर(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        माप_प्रकार consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        परिणाम = Curl_httpchunk_read(तथ्य, &ts->ch, &byte, 1, &consumed);
        अगर(परिणाम)
          वापस परिणाम;
        अगर(Curl_httpchunk_is_done(तथ्य, &ts->ch)) {
          /* we are done reading chunks! */
          infof(तथ्य, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      अन्यथा अगर(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        अगर(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          अवरोध;
        }
      }
      जारी;
    }

    अगर(ts->maybe_folded) {
      अगर(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      अन्यथा {
        परिणाम = single_header(cf, तथ्य, ts);
        अगर(परिणाम)
          वापस परिणाम;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    अगर(ts->leading_unfold) {
      अगर(ISBLANK(byte))
        /* skip a bit brother */
        जारी;
      /* non-blank, insert a space then continue the unfolding */
      अगर(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(तथ्य, "CONNECT response too large");
        वापस CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    अगर(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(तथ्य, "CONNECT response too large");
      वापस CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    अगर(byte != 0x0a)
      जारी;
    अन्यथा {
      स्थिर अक्षर *linep = curlx_dyn_ptr(&ts->rcvbuf);
      माप_प्रकार hlen = curlx_dyn_len(&ts->rcvbuf);
      अगर(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        परिणाम = single_header(cf, तथ्य, ts);
        अगर(परिणाम)
          वापस परिणाम;
      }
      अन्यथा
        ts->maybe_folded = TRUE;
    }

    अगर(परिणाम)
      वापस परिणाम;
  } /* while there is buffer left and loop is requested */

  अगर(error)
    परिणाम = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  अगर(!परिणाम && *done &&
     तथ्य->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && तथ्य->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    परिणाम = Curl_http_auth_act(तथ्य);
  }
  वापस परिणाम;
}

अटल CURLcode H1_CONNECT(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           काष्ठा h1_tunnel_state *ts)
{
  काष्ठा connectdata *conn = cf->conn;
  CURLcode परिणाम;
  bool done;

  अगर(tunnel_is_established(ts))
    वापस CURLE_OK;
  अगर(tunnel_is_failed(ts))
    वापस CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  करो {

    अगर(Curl_timeleft_ms(तथ्य) < 0) {
      failf(तथ्य, "Proxy CONNECT aborted due to timeout");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      जाओ out;
    }

    चयन(ts->tunnel_state) {
    हाल H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(तथ्य, cf, "CONNECT start");
      परिणाम = start_CONNECT(cf, तथ्य, ts);
      अगर(परिणाम)
        जाओ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, तथ्य);
      FALLTHROUGH();

    हाल H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(तथ्य, cf, "CONNECT send");
      परिणाम = send_CONNECT(cf, तथ्य, ts, &done);
      अगर(परिणाम || !done)
        जाओ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, तथ्य);
      FALLTHROUGH();

    हाल H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(तथ्य, cf, "CONNECT receive");
      परिणाम = recv_CONNECT_resp(cf, तथ्य, ts, &done);
      अगर(परिणाम)
        CURL_TRC_CF(तथ्य, cf, "error receiving CONNECT response: %d",
                    (पूर्णांक)परिणाम);
      अगर(!परिणाम)
        परिणाम = Curl_pgrsUpdate(तथ्य);
      /* error or not complete yet. return for more multi-multi */
      अगर(परिणाम || !done)
        जाओ out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, तथ्य);
      FALLTHROUGH();

    हाल H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(तथ्य, cf, "CONNECT response");
      अगर(तथ्य->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&तथ्य->req, तथ्य);
        अगर(ts->close_connection || conn->bits.बंद) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(तथ्य, cf, "CONNECT need to close+open");
          infof(तथ्य, "Connect me again please");
          वापस CURLE_AGAIN;
        }
        अन्यथा {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, तथ्य);
        }
      }
      अवरोध;

    शेष:
      अवरोध;
    }

  } जबतक(तथ्य->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  अगर(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    अगर(तथ्य->info.httpproxycode / 100 != 2 &&
       तथ्य->info.httpproxycode != 101) {
      curlx_safefree(तथ्य->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, तथ्य);
      failf(तथ्य, "CONNECT-UDP tunnel failed, response %d",
            तथ्य->req.httpcode);
      वापस CURLE_COULDNT_CONNECT;
    }
  }
  अन्यथा {
    अगर(तथ्य->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(तथ्य->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, तथ्य);
      failf(तथ्य, "CONNECT tunnel failed, response %d", तथ्य->req.httpcode);
      वापस CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, तथ्य);
  infof(तथ्य, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", तथ्य->info.httpproxycode);
  परिणाम = CURLE_OK;

out:
  अगर(परिणाम)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, तथ्य);
  वापस परिणाम;
}

अटल CURLcode cf_h1_proxy_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *done)
{
  CURLcode परिणाम;
  काष्ठा cf_h1_proxy_ctx *pctx = cf->ctx;
  काष्ठा h1_tunnel_state *ts = pctx->ts;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  CURL_TRC_CF(तथ्य, cf, "connect");
  परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, तथ्य, done);
  अगर(परिणाम || !*done)
    वापस परिणाम;

  *done = FALSE;
  अगर(!ts) {
    परिणाम = tunnel_init(cf, तथ्य, &ts);
    अगर(परिणाम)
      वापस परिणाम;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  परिणाम = H1_CONNECT(cf, तथ्य, ts);
  अगर(परिणाम)
    जाओ out;
  curlx_safefree(तथ्य->req.hd_proxy_auth);

out:
  *done = (परिणाम == CURLE_OK) && tunnel_is_established(pctx->ts);
  अगर(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&तथ्य->req, तथ्य);
    Curl_client_reset(तथ्य);
    Curl_pgrsReset(तथ्य);
    cf_tunnel_free(cf, तथ्य);
  }
  वापस परिणाम;
}

अटल CURLcode cf_h1_proxy_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा easy_pollset *ps)
{
  काष्ठा cf_h1_proxy_ctx *pctx = cf->ctx;
  काष्ठा h1_tunnel_state *ts = pctx->ts;
  CURLcode परिणाम = CURLE_OK;

  अगर(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, तथ्य);
    अगर(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      अगर(tunnel_want_send(ts))
        परिणाम = Curl_pollset_set_out_only(तथ्य, ps, sock);
      अन्यथा
        परिणाम = Curl_pollset_set_in_only(तथ्य, ps, sock);
    }
    अन्यथा
      परिणाम = Curl_pollset_set_out_only(तथ्य, ps, sock);
  }
  अन्यथा {
    अगर(cf->अग्र)
      परिणाम = cf->अग्र->cft->adjust_pollset(cf->अग्र, तथ्य, ps);
  }
  वापस परिणाम;
}

अटल bool cf_h1_proxy_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                     स्थिर काष्ठा Curl_easy *तथ्य)
{
  वापस cf->अग्र ? cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य) : FALSE;
}

अटल व्योम cf_h1_proxy_destroy(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(तथ्य, cf, "destroy");
  अगर(pctx) {
    cf_tunnel_free(cf, तथ्य);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

अटल CURLcode cf_h1_proxy_query(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_h1_proxy_ctx *pctx = cf->ctx;
  काष्ठा h1_tunnel_state *ts = pctx ? pctx->ts : शून्य;
  चयन(query) {
  हाल CF_QUERY_HOST_PORT:
    अगर(!ts || !ts->dest)
      अवरोध;
    *pres1 = (पूर्णांक)ts->dest->port;
    *((स्थिर अक्षर **)pres2) = ts->dest->hostname;
    वापस CURLE_OK;
  हाल CF_QUERY_ALPN_NEGOTIATED: {
    स्थिर अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = शून्य;
    वापस CURLE_OK;
  }
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

काष्ठा Curl_cftype Curl_cft_h1_proxy = {
  "H1-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  0,
  cf_h1_proxy_destroy,
  cf_h1_proxy_connect,
  Curl_cf_def_shutdown,
  cf_h1_proxy_adjust_pollset,
  cf_h1_proxy_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h1_proxy_query,
};

CURLcode Curl_cf_h1_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_peer *peer,
                                       काष्ठा Curl_peer *dest,
                                       पूर्णांक httpversion,
                                       bool udp_tunnel)
{
  काष्ठा Curl_cfilter *cf;
  काष्ठा cf_h1_proxy_ctx *pctx;
  काष्ठा h1_tunnel_state *ts;
  CURLcode परिणाम;

  (व्योम)तथ्य;
  अगर(!dest)
    वापस CURLE_FAILED_INIT;
  अगर((httpversion < 10) || (httpversion >= 20))
    वापस CURLE_FAILED_INIT;

  ts = curlx_calloc(1, माप(*ts));
  अगर(!ts) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_तथ्य, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(तथ्य, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, माप(*pctx));
  अगर(!pctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  परिणाम = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  अगर(परिणाम) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    जाओ out;
  }
  ts = शून्य;
  Curl_conn_cf_जोड़ो_after(cf_at, cf);

out:
  tunnel_free(ts, तथ्य);
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
