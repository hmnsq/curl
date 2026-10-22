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

#सचे !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)


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

प्रकारनिर्देश गणना {
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
  size_t nsent;
  size_t headerlines;
  संरचना Curl_chunker ch;
  पूर्णाङ्क httpversion;
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
  पटिनिवत्त ts && (ts->tunnel_state == H1_TUNNEL_ESTABLISHED);
}

स्थिर bool tunnel_is_failed(संरचना h1_tunnel_state *ts)
{
  पटिनिवत्त ts && (ts->tunnel_state == H1_TUNNEL_FAILED);
}

स्थिर bool h1_proxy_is_udp(संरचना Curl_cfilter *cf)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  पटिनिवत्त (bool)(pctx->ts && pctx->ts->udp_tunnel);
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
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode tunnel_init(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            संरचना h1_tunnel_state **pts)
{
  संरचना h1_tunnel_state *ts;

  सचे(cf->conn->scheme->flags & PROTOPT_NOTCPPROXY) {
    failf(data, "%s cannot be done over CONNECT", cf->conn->scheme->name);
    पटिनिवत्त CURLE_UNSUPPORTED_PROTOCOL;
  }

  ts = curlx_calloc(1, परिमाण(*ts));
  सचे(!ts)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  infof(data, "allocate connect buffer");

  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  *pts = ts;
  पटिनिवत्त tunnel_reinit(cf, data, ts);
}

स्थिर शून्य h1_tunnel_go_state(संरचना Curl_cfilter *cf,
                               संरचना h1_tunnel_state *ts,
                               h1_tunnel_state new_state,
                               संरचना Curl_easy *data)
{
  सचे(ts->tunnel_state == new_state)
    पटिनिवत्त;
  /* entering this one */
  विकल्प(new_state) {
  पक्ष H1_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "new tunnel state 'init'");
    tunnel_reinit(cf, data, ts);
    भङ्ग;

  पक्ष H1_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "new tunnel state 'connect'");
    ts->tunnel_state = H1_TUNNEL_CONNECT;
    ts->keepon = KEEPON_CONNECT;
    curlx_dyn_reset(&ts->rcvbuf);
    भङ्ग;

  पक्ष H1_TUNNEL_RECEIVE:
    CURL_TRC_CF(data, cf, "new tunnel state 'receive'");
    ts->tunnel_state = H1_TUNNEL_RECEIVE;
    भङ्ग;

  पक्ष H1_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "new tunnel state 'response'");
    ts->tunnel_state = H1_TUNNEL_RESPONSE;
    भङ्ग;

  पक्ष H1_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "new tunnel state 'established'");
    infof(data, "CONNECT%s phase completed for HTTP proxy",
          h1_proxy_is_udp(cf) ? "-UDP" : "");

    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  पक्ष H1_TUNNEL_FAILED:
    सचे(new_state == H1_TUNNEL_FAILED)
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
    भङ्ग;
  }
}

स्थिर शून्य tunnel_free(संरचना h1_tunnel_state *ts,
                        संरचना Curl_easy *data)
{
  सचे(ts) {
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
  सचे(cf) {
    संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
    संरचना h1_tunnel_state *ts = pctx ? pctx->ts : रिक्त;
    सचे(ts) {
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      tunnel_free(ts, data);
      pctx->ts = रिक्त;
    }
  }
}

स्थिर bool tunnel_want_send(संरचना h1_tunnel_state *ts)
{
  पटिनिवत्त ts->tunnel_state == H1_TUNNEL_CONNECT;
}

स्थिर CURLcode start_CONNECT(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              संरचना h1_tunnel_state *ts)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना httpreq *req = रिक्त;
  पूर्णाङ्क http_minor;
  CURLcode result;

  DEBUGASSERT(data);
  /* This only happens if we have looped here due to authentication reasons,
     and we do not really use the newly cloned URL here then. Free it. */
  curlx_safefree(data->req.newurl);

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 pctx->peer, ts->dest,
                                                 PROXY_HTTP_V1,
                                                 h1_proxy_is_udp(cf));
  सचे(result)
    गच्छ out;

  curlx_dyn_reset(&ts->request_data);
  ts->nsent = 0;
  ts->headerlines = 0;
  http_minor = ts->httpversion % 10;

  result = Curl_h1_req_write_head(req, http_minor, &ts->request_data);
  सचे(!result)
    result = Curl_creader_set_null(data);

out:
  सचे(result)
    failf(data, "Failed sending CONNECT to proxy");
  सचे(req)
    Curl_http_req_free(req);
  पटिनिवत्त result;
}

स्थिर CURLcode send_CONNECT(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             संरचना h1_tunnel_state *ts,
                             bool *done)
{
  ध्रुव uint8_t *buf = curlx_dyn_uptr(&ts->request_data);
  size_t request_len = curlx_dyn_len(&ts->request_data);
  size_t blen = request_len;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  सचे(blen <= ts->nsent)
    गच्छ out;  /* we are done */

  blen -= ts->nsent;
  buf += ts->nsent;

  result = cf->next->cft->do_send(cf->next, data, buf, blen, FALSE, &nwritten);
  सचे(result) {
    सचे(result == CURLE_AGAIN)
      result = CURLE_OK;
    गच्छ out;
  }

  DEBUGASSERT(blen >= nwritten);
  ts->nsent += nwritten;
  Curl_debug(data, CURLINFO_HEADER_OUT, (ध्रुव वर्ण *)buf, nwritten);

out:
  सचे(result)
    failf(data, "Failed sending CONNECT to proxy");
  *done = (!result && (ts->nsent >= request_len));
  पटिनिवत्त result;
}

स्थिर CURLcode on_resp_status(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना h1_tunnel_state *ts,
                               ध्रुव वर्ण *header)
{
  संरचना SingleRequest *k = &data->req;

  (शून्य)ts;
  सचे(!strncmp(header, "HTTP/1.", 7) &&
          ((header[7] == '0') || (header[7] == '1')) &&
          (header[8] == ' ') &&
          ISDIGIT(header[9]) && ISDIGIT(header[10]) && ISDIGIT(header[11]) &&
          !ISDIGIT(header[12])) {
    /* store the HTTP code from the proxy */
    data->info.httpproxycode = k->httpcode = ((header[9] - '0') * 100) +
      ((header[10] - '0') * 10) + (header[11] - '0');
    CURL_TRC_CF(data, cf, "CONNECT%s HTTP status %d",
                h1_proxy_is_udp(cf) ? "-UDP" : "", k->httpcode);
    पटिनिवत्त CURLE_OK;
  }
  failf(data, "Invalid response header");
  पटिनिवत्त CURLE_WEIRD_SERVER_REPLY;
}

स्थिर CURLcode on_resp_header(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना h1_tunnel_state *ts,
                               ध्रुव वर्ण *header)
{
  CURLcode result = CURLE_OK;
  संरचना SingleRequest *k = &data->req;
  bool is_udp = h1_proxy_is_udp(cf);

  सचे(checkprefix("Proxy-authenticate:", header) && (407 == k->httpcode)) {

    bool proxy = (k->httpcode == 407);
    वर्ण *auth = Curl_copy_header_मानम्(header);
    सचे(!auth)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;

    CURL_TRC_CF(data, cf, "CONNECT%s: fwd auth header '%s'",
                is_udp ? "-UDP" : "", header);
    result = Curl_http_निवेशः_auth(data, proxy, auth);

    curlx_free(auth);

    सचे(result)
      पटिनिवत्त result;
  }
  अञ्ञथा सचे(checkprefix("Content-Length:", header)) {
    सचे(k->httpcode < 300) {
      सचे(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Content-Length: in %03d response", k->httpcode);
        पटिनिवत्त CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Content-Length in CONNECT%s %03d response",
            is_udp ? "-UDP" : "", k->httpcode);
    }
    अञ्ञथा {
      ध्रुव वर्ण *p = header + CURL_CSTRLEN("Content-Length:");
      सचे(curlx_str_numblanks(&p, &ts->cl)) {
        failf(data, "Unsupported Content-Length value");
        पटिनिवत्त CURLE_WEIRD_SERVER_REPLY;
      }
    }
  }
  अञ्ञथा सचे(Curl_compareheader(header,
                             STRCONST("Connection:"), STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Connection: close", is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  अञ्ञथा सचे(checkprefix("Transfer-Encoding:", header)) {
    सचे(k->httpcode < 300) {
      सचे(k->httpcode < 200) {
        /* Informational 1xx responses cannot carry a body. RFC 9110 15.2 */
        failf(data, "Invalid Transfer-Encoding: in %03d response",
              k->httpcode);
        पटिनिवत्त CURLE_WEIRD_SERVER_REPLY;
      }
      /* A client MUST ignore any Content-Length or Transfer-Encoding
         header fields received in a successful response to CONNECT.
         "Successful" described as: 2xx (Successful). RFC 7231 4.3.6 */
      infof(data, "Ignoring Transfer-Encoding in "
            "CONNECT%s %03d response", is_udp ? "-UDP" : "", k->httpcode);
    }
    अञ्ञथा सचे(Curl_compareheader(header,
                               STRCONST("Transfer-Encoding:"),
                               STRCONST("chunked"))) {
      CURL_TRC_CF(data, cf, "CONNECT%s response chunked",
                  is_udp ? "-UDP" : "");
      ts->chunked_encoding = TRUE;
      Curl_httpchunk_reset(data, &ts->ch, TRUE);
    }
  }
  अञ्ञथा सचे(is_udp && checkprefix("Capsule-protocol:", header)) {
    सचे(Curl_compareheader(header,
                           STRCONST("Capsule-protocol:"),
                           STRCONST("?1"))) {
      CURL_TRC_CF(data, cf, "CONNECT-UDP Response --> Capsule-protocol: ?1");
    }
  }
  अञ्ञथा सचे(Curl_compareheader(header,
                             STRCONST("Proxy-Connection:"),
                             STRCONST("close"))) {
    CURL_TRC_CF(data, cf, "CONNECT%s Proxy-Connection: close",
                is_udp ? "-UDP" : "");
    ts->close_connection = TRUE;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode single_header(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              संरचना h1_tunnel_state *ts)
{
  CURLcode result = CURLE_OK;
  ध्रुव वर्ण *linep = curlx_dyn_ptr(&ts->rcvbuf);
  size_t पङ्क्तिः_len = curlx_dyn_len(&ts->rcvbuf); /* bytes in this line */
  ध्रुव संरचना SingleRequest *k = &data->req;
  पूर्णाङ्क writetype;
  ts->headerlines++;

  /* output debug if that is requested */
  Curl_debug(data, CURLINFO_HEADER_IN, linep, पङ्क्तिः_len);

  /* a CONNECT response line is handed to the client as a header, so it must
     pass the same checks as a regular response header before delivery */
  result = Curl_verify_header(data, linep, पङ्क्तिः_len);
  सचे(result)
    पटिनिवत्त result;

  /* send the header to the callback */
  writetype = CLIENTWRITE_HEADER | CLIENTWRITE_CONNECT |
    (ts->headerlines == 1 ? CLIENTWRITE_STATUS : 0);
  result = Curl_client_write(data, writetype, linep, पङ्क्तिः_len);
  सचे(result)
    पटिनिवत्त result;

  result = Curl_bump_headersize(data, पङ्क्तिः_len, TRUE);
  सचे(result)
    पटिनिवत्त result;

  /* Newlines are CRLF, so the CR is ignored as the line is not
     really terminated until the LF comes. Treat a following CR
     as end-of-headers as well.*/

  सचे(ISNEWLINE(linep[0])) {
    /* end of response-headers from the proxy */
    सचे(ts->headerlines == 1) {
      ts->keepon = KEEPON_DONE;
      result = CURLE_WEIRD_SERVER_REPLY;
    }
    अञ्ञथा सचे((407 == k->httpcode) && !data->state.authproblem) {
      /* If we get a 407 response code with content length
         when we have no auth problem, we must ignore the
         whole response-body */
      ts->keepon = KEEPON_IGNORE;

      सचे(ts->cl) {
        infof(data, "Ignore %" FMT_OFF_T " bytes of response-body", ts->cl);
      }
      अञ्ञथा सचे(ts->chunked_encoding) {
        infof(data, "Ignore chunked response-body");
      }
      अञ्ञथा {
        /* without content-length or chunked encoding, we
           cannot keep the connection alive since the close is
           the end signal so we bail out at once instead */
        CURL_TRC_CF(data, cf, "CONNECT: no content-length or chunked");
        ts->keepon = KEEPON_DONE;
      }
    }
    अञ्ञथा {
      ts->keepon = KEEPON_DONE;
    }

    DEBUGASSERT(ts->keepon == KEEPON_IGNORE ||
                ts->keepon == KEEPON_DONE);
    पटिनिवत्त result;
  }

  सचे(ts->headerlines == 1)
    result = on_resp_status(cf, data, ts, linep);
  अञ्ञथा
    result = on_resp_header(cf, data, ts, linep);
  सचे(result)
    पटिनिवत्त result;

  curlx_dyn_reset(&ts->rcvbuf);
  पटिनिवत्त result;
}

स्थिर CURLcode recv_CONNECT_resp(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना h1_tunnel_state *ts,
                                  bool *done)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क error;

#define SELECT_OK      0
#define SELECT_ERROR   1

  error = SELECT_OK;
  *done = FALSE;

  याव(ts->keepon) {
    size_t nread;
    वर्ण byte;

    /* Read one byte at a time to avoid a race condition. Wait at most one
       second before looping to ensure continuous pgrsUpdates. */
    result = Curl_conn_recv(data, cf->sockindex, &byte, 1, &nread);
    सचे(result == CURLE_AGAIN)
      /* socket buffer drained, return */
      पटिनिवत्त CURLE_OK;

    सचे(!result)
      result = Curl_pgrsUpdate(data);

    सचे(result) {
      ts->keepon = KEEPON_DONE;
      भङ्ग;
    }

    सचे(!nread) {
      सचे(ts->maybe_folded) {
        /* EOF right after LF: finalize the pending header line. */
        result = single_header(cf, data, ts);
        सचे(result)
          पटिनिवत्त result;
        ts->maybe_folded = FALSE;
      }
      सचे(data->set.proxyauth && data->state.authproxy.avail &&
         data->req.hd_proxy_auth) {
        /* proxy auth was requested and there was proxy auth available,
           then deem this as "mere" proxy disconnect */
        ts->close_connection = TRUE;
        infof(data, "Proxy CONNECT connection closed");
      }
      अञ्ञथा {
        error = SELECT_ERROR;
        failf(data, "Proxy CONNECT aborted");
      }
      ts->keepon = KEEPON_DONE;
      भङ्ग;
    }

    सचे(ts->keepon == KEEPON_IGNORE) {
      /* This means we are currently ignoring a response-body */
      सचे(ts->chunked_encoding) {
        /* chunked-encoded body, so we need to do the chunked dance
           properly to know when the end of the body is reached */
        size_t consumed = 0;

        /* now parse the chunked piece of data so that we can
           properly tell when the stream ends */
        result = Curl_httpchunk_read(data, &ts->ch, &byte, 1, &consumed);
        सचे(result)
          पटिनिवत्त result;
        सचे(Curl_httpchunk_is_done(data, &ts->ch)) {
          /* we are done reading chunks! */
          infof(data, "chunk reading DONE");
          ts->keepon = KEEPON_DONE;
        }
      }
      अञ्ञथा सचे(ts->cl) {
        /* A Content-Length based body: count down the counter
           and make sure to break out of the loop when we are done! */
        ts->cl--;
        सचे(ts->cl <= 0) {
          ts->keepon = KEEPON_DONE;
          भङ्ग;
        }
      }
      अनुवर्त;
    }

    सचे(ts->maybe_folded) {
      सचे(ISBLANK(byte)) {
        Curl_http_to_fold(&ts->rcvbuf);
        ts->leading_unfold = TRUE;
      }
      अञ्ञथा {
        result = single_header(cf, data, ts);
        सचे(result)
          पटिनिवत्त result;
        /* now handle the new byte */
      }
      ts->maybe_folded = FALSE;
    }

    सचे(ts->leading_unfold) {
      सचे(ISBLANK(byte))
        /* skip a bit brother */
        अनुवर्त;
      /* non-blank, insert a space then continue the unfolding */
      सचे(curlx_dyn_addn(&ts->rcvbuf, " ", 1)) {
        failf(data, "CONNECT response too large");
        पटिनिवत्त CURLE_RECV_ERROR;
      }
      ts->leading_unfold = FALSE;
    }
    सचे(curlx_dyn_addn(&ts->rcvbuf, &byte, 1)) {
      failf(data, "CONNECT response too large");
      पटिनिवत्त CURLE_RECV_ERROR;
    }

    /* if this is not the end of a header line then continue */
    सचे(byte != 0x0a)
      अनुवर्त;
    अञ्ञथा {
      ध्रुव वर्ण *linep = curlx_dyn_ptr(&ts->rcvbuf);
      size_t hlen = curlx_dyn_len(&ts->rcvbuf);
      सचे(hlen && ISNEWLINE(linep[0])) {
        /* end of headers */
        result = single_header(cf, data, ts);
        सचे(result)
          पटिनिवत्त result;
      }
      अञ्ञथा
        ts->maybe_folded = TRUE;
    }

    सचे(result)
      पटिनिवत्त result;
  } /* while there is buffer left and loop is requested */

  सचे(error)
    result = CURLE_RECV_ERROR;
  *done = (ts->keepon == KEEPON_DONE);
  सचे(!result && *done &&
     data->info.httpproxycode / 100 != 2 &&
     !(h1_proxy_is_udp(cf) && data->info.httpproxycode == 101)) {
    /* Deal with the possibly already received authenticate
       headers. 'newurl' is set to a new URL if we must loop. */
    result = Curl_http_auth_act(data);
  }
  पटिनिवत्त result;
}

स्थिर CURLcode H1_CONNECT(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना h1_tunnel_state *ts)
{
  संरचना connectdata *conn = cf->conn;
  CURLcode result;
  bool done;

  सचे(tunnel_is_established(ts))
    पटिनिवत्त CURLE_OK;
  सचे(tunnel_is_failed(ts))
    पटिनिवत्त CURLE_RECV_ERROR; /* Need a cfilter close and new bootstrap */

  करोहि {

    सचे(Curl_timeleft_ms(data) < 0) {
      failf(data, "Proxy CONNECT aborted due to timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      गच्छ out;
    }

    विकल्प(ts->tunnel_state) {
    पक्ष H1_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "CONNECT start");
      result = start_CONNECT(cf, data, ts);
      सचे(result)
        गच्छ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_CONNECT, data);
      FALLTHROUGH();

    पक्ष H1_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      CURL_TRC_CF(data, cf, "CONNECT send");
      result = send_CONNECT(cf, data, ts, &done);
      सचे(result || !done)
        गच्छ out;
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RECEIVE, data);
      FALLTHROUGH();

    पक्ष H1_TUNNEL_RECEIVE:
      /* read what is there */
      CURL_TRC_CF(data, cf, "CONNECT receive");
      result = recv_CONNECT_resp(cf, data, ts, &done);
      सचे(result)
        CURL_TRC_CF(data, cf, "error receiving CONNECT response: %d",
                    (पूर्णाङ्क)result);
      सचे(!result)
        result = Curl_pgrsUpdate(data);
      /* error or not complete yet. return for more multi-multi */
      सचे(result || !done)
        गच्छ out;
      /* got it */
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_RESPONSE, data);
      FALLTHROUGH();

    पक्ष H1_TUNNEL_RESPONSE:
      CURL_TRC_CF(data, cf, "CONNECT response");
      सचे(data->req.newurl) {
        /* not the "final" response, we need to do a follow up request.
         * If the other side indicated a connection close, or if someone
         * else told us to close this connection, do so now.
         */
        Curl_req_soft_reset(&data->req, data);
        सचे(ts->close_connection || conn->bits.close) {
          /* Close this filter and the sub-chain, re-connect the
           * sub-chain and continue. Closing this filter will
           * reset our tunnel state. To avoid recursion, we return
           * and expect to be called again.
           */
          CURL_TRC_CF(data, cf, "CONNECT need to close+open");
          infof(data, "Connect me again please");
          पटिनिवत्त CURLE_AGAIN;
        }
        अञ्ञथा {
          /* staying on this connection, reset state */
          h1_tunnel_go_state(cf, ts, H1_TUNNEL_INIT, data);
        }
      }
      भङ्ग;

    स्वभाव:
      भङ्ग;
    }

  } याव(data->req.newurl);

  DEBUGASSERT(ts->tunnel_state == H1_TUNNEL_RESPONSE);
  सचे(h1_proxy_is_udp(cf)) {
    /* RFC 9298: Accept 101 Upgrade for HTTP/1.1 and
     * 2xx responses for HTTP/2 and HTTP/3 proxies. */
    सचे(data->info.httpproxycode / 100 != 2 &&
       data->info.httpproxycode != 101) {
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT-UDP tunnel failed, response %d",
            data->req.httpcode);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }
  }
  अञ्ञथा {
    सचे(data->info.httpproxycode / 100 != 2) {
      /* a non-2xx response and we have no next URL to try. */
      curlx_safefree(data->req.newurl);
      h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
      failf(data, "CONNECT tunnel failed, response %d", data->req.httpcode);
      पटिनिवत्त CURLE_COULDNT_CONNECT;
    }
  }
  /* 2xx response, SUCCESS! */
  /* 101 Switching Protocol for CONNECT-UDP */
  h1_tunnel_go_state(cf, ts, H1_TUNNEL_ESTABLISHED, data);
  infof(data, "CONNECT%s tunnel established, response %d",
        h1_proxy_is_udp(cf) ? "-UDP" : "", data->info.httpproxycode);
  result = CURLE_OK;

out:
  सचे(result)
    h1_tunnel_go_state(cf, ts, H1_TUNNEL_FAILED, data);
  पटिनिवत्त result;
}

स्थिर CURLcode cf_h1_proxy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  CURLcode result;
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx->ts;

  सचे(cf->connected) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  CURL_TRC_CF(data, cf, "connect");
  result = cf->next->cft->do_connect(cf->next, data, done);
  सचे(result || !*done)
    पटिनिवत्त result;

  *done = FALSE;
  सचे(!ts) {
    result = tunnel_init(cf, data, &ts);
    सचे(result)
      पटिनिवत्त result;
    pctx->ts = ts;
  }

  /* We want "seamless" operations through HTTP proxy tunnel */

  result = H1_CONNECT(cf, data, ts);
  सचे(result)
    गच्छ out;
  curlx_safefree(data->req.hd_proxy_auth);

out:
  *done = (result == CURLE_OK) && tunnel_is_established(pctx->ts);
  सचे(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
    Curl_pgrsReset(data);
    cf_tunnel_free(cf, data);
  }
  पटिनिवत्त result;
}

स्थिर CURLcode cf_h1_proxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx->ts;
  CURLcode result = CURLE_OK;

  सचे(!cf->connected) {
    /* If we are not connected, but the filter "below" is
     * and not waiting on something, we are tunneling. */
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    सचे(ts) {
      /* when we have sent a CONNECT to a proxy, we should rather either
         wait for the socket to become readable to be able to get the
         response headers or if we are still sending the request, wait
         for write. */
      सचे(tunnel_want_send(ts))
        result = Curl_pollset_set_out_only(data, ps, sock);
      अञ्ञथा
        result = Curl_pollset_set_in_only(data, ps, sock);
    }
    अञ्ञथा
      result = Curl_pollset_set_out_only(data, ps, sock);
  }
  अञ्ञथा {
    सचे(cf->next)
      result = cf->next->cft->adjust_pollset(cf->next, data, ps);
  }
  पटिनिवत्त result;
}

स्थिर bool cf_h1_proxy_data_pending(संरचना Curl_cfilter *cf,
                                     ध्रुव संरचना Curl_easy *data)
{
  पटिनिवत्त cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

स्थिर शून्य cf_h1_proxy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  सचे(pctx) {
    cf_tunnel_free(cf, data);
    Curl_peer_unlink(&pctx->peer);
    curlx_safefree(cf->ctx);
  }
}

स्थिर CURLcode cf_h1_proxy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_h1_proxy_ctx *pctx = cf->ctx;
  संरचना h1_tunnel_state *ts = pctx ? pctx->ts : रिक्त;
  विकल्प(query) {
  पक्ष CF_QUERY_HOST_PORT:
    सचे(!ts || !ts->dest)
      भङ्ग;
    *pres1 = (पूर्णाङ्क)ts->dest->port;
    *((ध्रुव वर्ण **)pres2) = ts->dest->hostname;
    पटिनिवत्त CURLE_OK;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    पटिनिवत्त CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  पटिनिवत्त cf->next ?
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

CURLcode Curl_cf_h1_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *data,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *dest,
                                       पूर्णाङ्क httpversion,
                                       bool udp_tunnel)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_h1_proxy_ctx *pctx;
  संरचना h1_tunnel_state *ts;
  CURLcode result;

  (शून्य)data;
  सचे(!dest)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे((httpversion < 10) || (httpversion >= 20))
    पटिनिवत्त CURLE_FAILED_INIT;

  ts = curlx_calloc(1, परिमाण(*ts));
  सचे(!ts) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  Curl_peer_link(&ts->dest, dest);
  ts->udp_tunnel = udp_tunnel;
  ts->httpversion = httpversion;
  curlx_dyn_init(&ts->rcvbuf, DYN_PROXY_CONNECT_HEADERS);
  curlx_dyn_init(&ts->request_data, DYN_HTTP_REQUEST);
  Curl_httpchunk_init(data, &ts->ch, TRUE, TRUE);

  pctx = curlx_calloc(1, परिमाण(*pctx));
  सचे(!pctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  Curl_peer_link(&pctx->peer, peer);
  pctx->ts = ts;
  result = Curl_cf_create(&cf, &Curl_cft_h1_proxy, pctx);
  सचे(result) {
    Curl_peer_unlink(&pctx->peer);
    curlx_free(pctx);
    गच्छ out;
  }
  ts = रिक्त;
  Curl_conn_cf_निवेशय_after(cf_at, cf);

out:
  tunnel_free(ts, data);
  पटिनिवत्त result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
