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

#जर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


#अन्तर्भाव <curl/curl.h>
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-h1-proxy.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curlx/strparse.h"

प्रकारव्याख्या गणना {
  H1_TUNNEL_INIT,     /* init/default/no tunnel state */
  H1_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H1_TUNNEL_RECEIVE,  /* CONNECT answer is being received */
  H1_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H1_TUNNEL_ESTABLISHED,
  H1_TUNNEL_FAILED
} h1_tunnel_state;

/* struct for HTTP CONNECT tunneling */
संरचना h1_tunnel_state {
  संरचना Curl_peer *dest;
  संरचना dynbuf rcvbuf;
  संरचना dynbuf request_data;
  माप_प्रकार nsent;
  माप_प्रकार headerlines;
  संरचना Curl_chunker ch;
  पूर्णांक httpversion;
  गणना keeponval {
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
संरचना cf_h1_proxy_ctx {
  संरचना Curl_peer *peer;
  संरचना h1_tunnel_state *ts;
};

स्थिर bool tunnel_is_established(संरचना h1_tunnel_state *ts)
{
  परत ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

स्थिर bool tunnel_is_failed(संरचना h1_tunnel_state *ts)
{
  परत ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

स्थिर bool h1_proxy_is_udp(संरचना Curl_cfilter *cf)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  परत (bool)(pctx->ts && pctx->ts->udp_tunnel);
}

स्थिर CURLcode tunnel_reinit(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              संरचना h1_tunnel_state *ts)
{
  (शून्य)data;
  (शून्य)cf;
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
  परत CURLE_OK;
}

स्थिर CURLcode tunnel_init(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            संरचना h1_tunnel_state **pts)
{
  संरचना h1_tunnel_state *ts;

  जर(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    परत CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, आकार(*ts));
  जर(!ts)
    परत CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  परत tunnel_reinit(cf, data, ts);
}

स्थिर शून्य h1_tunnel_go_state(संरचना Curl_cfilter *cf,
                               संरचना h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               संरचना Curl_easy *data)
{
  जर(ts->tunnel_state == new_state)
    परत;
  /* entering this one */
  निवड(new_state) {
  स्थिती H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    थांबा;

  स्थिती H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    थांबा;

  स्थिती H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    थांबा;

  स्थिती H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    थांबा;

  स्थिती H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  स्थिती H1_TUNNEL_FAILED:
    जर(new_state == H1_TUNNEL_FAILED)
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
    थांबा;
  }
}

स्थिर शून्य tunnel_free(संरचना h1_tunnel_state *ts,
                        संरचना Curl_easy *data)
{
  जर(ts) {
    Curl_peer_unlink(&ts->dest);
    curlx_dyn_free(&ts->rcvbuf);
    curlx_dyn_free(&ts->request_data);
    Curl_httpchunk_free(data, &ts->ch);
    curlx_free(ts);
  }
}

स्थिर शून्य cf_tunnel_free(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data)
{
  जर(cf) {
    संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
    संरचना h1_tunnel_state *ts = pctx ? pctx->ts : रिक्त;
    जर(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = रिक्त;
    }
  }
}

स्थिर bool tunnel_want_send(संरचना h1_tunnel_state *ts)
{
  परत ts->tunnel_state == H1_TUNNEL_CONNECT;
}

स्थिर CURLcode start_CONNECT(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              संरचना h1_tunnel_state *ts)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना httpreq *req = रिक्त;
  पूर्णांक http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  जर(result)
    जा out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  जर(!result)
    result = Curl_creader_set_null(data);

out:
  जर(result)
    failf(data, "Failed sending CONNECT to proxy");
  जर(req)
    Curl_http_req_free(req);
  परत result;
}

स्थिर CURLcode send_CONNECT(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             संरचना h1_tunnel_state *ts,
                             bool *done)
{
  स्थिरांक uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  माप_प्रकार request_len = curlx_dyn_len(&ts->request_data);
  माप_प्रकार blen = request_len;
  CURLcode result = CURLE_OK;
  माप_प्रकार nwritten;

  जर(blen <= ts->nsent)
    जा out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  जर(result) {
    जर(result == CURLE_AGAIN)
      result = CURLE_OK;
    जा out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (स्थिरांक अक्षर *)buf, nwritten);

out:
  जर(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  परत result;
}

स्थिर CURLcode on_resp_status(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना h1_tunnel_state *ts,
                               स्थिरांक अक्षर *header)
{
  संरचना SingleRequest *k = &data->req;

  (शून्य)ts;
  जर(!म_भेदन(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    परत CURLE_OK;
  }
  failf(data, "Invalid response header");
  परत CURLE_WEIRD_SERVER_REPLY;
}

स्थिर CURLcode on_resp_header(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना h1_tunnel_state *ts,
                               स्थिरांक अक्षर *header)
{
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  जर(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    अक्षर *auth = Curl_copy_header_मूल्य(header);
    जर(!auth)
      परत CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_आदान_auth(data, proxy, auth);

    curlx_free(auth);

    जर(result)
      परत result;
  }
  अन्यथा जर(checkprefix("Content-Length:", header)) {
    जर(k->httpcode < 300) {
      जर(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        परत CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    अन्यथा {
      स्थिरांक अक्षर *p = header + CURL_CSTRLEN("Content-Length:");
      जर(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        परत CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  अन्यथा जर(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  अन्यथा जर(checkprefix("Transfer-Encoding:", header)) {
    जर(k->httpcode < 300) {
      जर(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        परत CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    अन्यथा जर(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  अन्यथा जर(is_udp && checkprefix("Capsule-protocol:", header)) {
    जर(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  अन्यथा जर(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  परत result;
}

स्थिर CURLcode single_header(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              संरचना h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  स्थिरांक अक्षर *linep = curlx_dyn_ptr(&ts->rcvbuf);
  माप_प्रकार ओळ_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  स्थिरांक संरचना SingleRequest *k = &data->req;
  पूर्णांक writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, ओळ_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, ओळ_len);
  जर(result)
    परत result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, ओळ_len);
  जर(result)
    परत result;

  result = Curl_bump_headersize(data, ओळ_len, TRUE);
  जर(result)
    परत result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  जर(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    जर(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    अन्यथा जर((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      जर(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      अन्यथा जर(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      अन्यथा {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    अन्यथा {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    परत result;
  }

  जर(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  अन्यथा
    result = on_resp_header(cf, data, ts, linep);
  जर(result)
    परत result;

  curlx_dyn_reset(&ts->rcvbuf);
  परत result;
}

स्थिर CURLcode recv_CONNECT_resp(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  पूर्णांक error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  पर्यंत(ts->keepon) {
    माप_प्रकार nread;
    अक्षर byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    जर(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      परत CURLE_OK;

    जर(!result)
      result = Curl_pgrsUpdate(data);

    जर(result) {
      ts->keepon = KEEPON_DONE;
      थांबा;
    }

    जर(!nread) {
      जर(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        जर(result)
          परत result;
        ts->maybe_folded = FALSE;
      }
      जर(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      अन्यथा {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      थांबा;
    }

    जर(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      जर(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        माप_प्रकार consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        जर(result)
          परत result;
        जर(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      अन्यथा जर(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        जर(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          थांबा;
        }
      }
      सुरूठेवा;
    }

    जर(ts->maybe_folded) {
      जर(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      अन्यथा {
        result = single_header(cf, data, ts);
        जर(result)
          परत result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    जर(ts->leading_unfold) {
      जर(ISBLANK(byte))
        /* skip a bit brother */
        सुरूठेवा;
      /* non-blank, insert a space then continue the unfolding */
      जर(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        परत CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    जर(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      परत CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    जर(byte != 0x0a)
      सुरूठेवा;
    अन्यथा {
      स्थिरांक अक्षर *linep = curlx_dyn_ptr(&ts->rcvbuf);
      माप_प्रकार hlen = curlx_dyn_len(&ts->rcvbuf);
      जर(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        जर(result)
          परत result;
      }
      अन्यथा
        ts->maybe_folded = TRUE;
    }

    जर(result)
      परत result;
  } /* while there is buffer left and loop is requested */

  जर(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  जर(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  परत result;
}

स्थिर CURLcode H1_CONNECT(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना h1_tunnel_state *ts)
{
  संरचना connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  जर(tunnel_is_established(ts))
    परत CURLE_OK;
  जर(tunnel_is_failed(ts))
    परत CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  करा {

    जर(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      जा out;
    }

    निवड(ts->tunnel_state) {
    स्थिती H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      जर(result)
        जा out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    स्थिती H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      जर(result || !done)
        जा out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    स्थिती H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      जर(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (पूर्णांक)result);
      जर(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      जर(result || !done)
        जा out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    स्थिती H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      जर(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        जर(ts->close_connection || conn->bits.बंद) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          परत CURLE_AGAIN;
        }
        अन्यथा {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      थांबा;

    पूर्वनिर्धारित:
      थांबा;
    }

  } पर्यंत(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  जर(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    जर(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      परत CURLE_COULDNT_CONNECT;
    }
  }
  अन्यथा {
    जर(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      परत CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  जर(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  परत result;
}

स्थिर CURLcode cf_h1_proxy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx->ts;

  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  जर(result || !*done)
    परत result;

  *done = FALSE;
  जर(!ts) {
    result = tunnel_init(cf, data, &ts);
    जर(result)
      परत result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  जर(result)
    जा out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  जर(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  परत result;
}

स्थिर CURLcode cf_h1_proxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  जर(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    जर(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      जर(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      अन्यथा
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    अन्यथा
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  अन्यथा {
    जर(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  परत result;
}

स्थिर bool cf_h1_proxy_data_pending(संरचना Curl_cfilter *cf,
                                     स्थिरांक संरचना Curl_easy *data)
{
  परत cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

स्थिर शून्य cf_h1_proxy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  जर(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

स्थिर CURLcode cf_h1_proxy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx ? pctx->ts : रिक्त;
  निवड(query) {
  स्थिती CF_QUERY_HOST_PORT:
    जर(!ts || !ts->dest)
      थांबा;
    *pres1 = (पूर्णांक)ts->dest->port;
    *((स्थिरांक अक्षर **)pres2) = ts->dest->hostname;
    परत CURLE_OK;
  स्थिती CF_QUERY_ALPN_NEGOTIATED: {
    स्थिरांक अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    परत CURLE_OK;
  }
  पूर्वनिर्धारित:
    थांबा;
  }
  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

संरचना Curl_cftype Curl_cft_h1_proxy = {
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

CURLcode Curl_cf_h1_proxy_घाला_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *data,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *dest,
                                       पूर्णांक httpversion,
                                       bool udp_tunnel)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_h1_proxy_ctx *pctx;
  संरचना h1_tunnel_state *ts;
  CURLcode result;

  (शून्य)data;
  जर(!dest)
    परत CURLE_FAILED_INIT;
  जर((httpversion < 10) || (httpversion >= 20))
    परत CURLE_FAILED_INIT;

  ts = curlx_calloc(1, आकार(*ts));
  जर(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, आकार(*pctx));
  जर(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  जर(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    जा out;
  }
  ts = रिक्त;
  Curl_conn_cf_घाला_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  परत result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
